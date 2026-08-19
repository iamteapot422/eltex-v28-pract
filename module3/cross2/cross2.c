#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/wait.h>

#define MAX_DRIVERS 64

static volatile sig_atomic_t g_stop = 0;

void handle_stop_signal(int sig)
{
    g_stop = 1;
}

void install_signal_handlers()
{
    struct sigaction sa = { .sa_handler = handle_stop_signal };
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    signal(SIGPIPE, SIG_IGN);
}

typedef struct
{
    pid_t pid;
    int in_fd;
    int out_fd;
} driver_ref_t;

static driver_ref_t drivers[MAX_DRIVERS];
static int ndrivers = 0;

int read_line(int fd, char* buf, int maxlen)
{
    int i = 0;
    while (i < maxlen - 1)
    {
        char c;
        ssize_t r = read(fd, &c, 1);
        if (r <= 0) return -1;
        if (c == '\n') break;
        buf[i++] = c;
    }
    buf[i] = '\0';
    return i;
}

void write_line(int fd, const char* text)
{
    char buf[128];
    int n = snprintf(buf, sizeof(buf), "%s\n", text);
    write(fd, buf, n);
}

void driver_main(const char* in_path, const char* out_path)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    int in_fd = open(in_path, O_RDWR);
    int out_fd = open(out_path, O_RDWR);

    time_t busy_until = 0;

    while (1)
    {
        struct timeval tv;
        struct timeval* tvp = NULL;

        if (busy_until != 0)
        {
            time_t now = time(NULL);
            long remaining = busy_until - now;
            if (remaining <= 0)
            {
                busy_until = 0;
            }
            else
            {
                tv.tv_sec = remaining;
                tv.tv_usec = 0;
                tvp = &tv;
            }
        }

        fd_set set;
        FD_ZERO(&set);
        FD_SET(in_fd, &set);

        int r = select(in_fd + 1, &set, NULL, NULL, tvp);
        if (r < 0)
        {
            if (errno == EINTR) continue;
            break;
        }
        if (r == 0)
        {
            busy_until = 0;
            continue;
        }

        char cmd[64];
        if (read_line(in_fd, cmd, sizeof(cmd)) < 0) break;

        if (strcmp(cmd, "STOP") == 0) break;

        time_t now = time(NULL);
        char reply[64];

        if (strncmp(cmd, "TASK ", 5) == 0)
        {
            if (busy_until != 0 && busy_until > now)
            {
                snprintf(reply, sizeof(reply), "Busy %ld", busy_until - now);
            }
            else
            {
                int secs = atoi(cmd + 5);
                busy_until = now + secs;
                snprintf(reply, sizeof(reply), "OK");
            }
        }
        else if (strcmp(cmd, "STATUS") == 0)
        {
            if (busy_until != 0 && busy_until > now)
                snprintf(reply, sizeof(reply), "Busy %ld", busy_until - now);
            else
                snprintf(reply, sizeof(reply), "Available");
        }
        else
        {
            snprintf(reply, sizeof(reply), "Unknown command");
        }

        write_line(out_fd, reply);
    }

    close(in_fd);
    close(out_fd);
    unlink(in_path);
    unlink(out_path);
}

int find_driver(int pid)
{
    for (int i = 0; i < ndrivers; i++)
    {
        if (drivers[i].pid == pid) return i;
    }
    return -1;
}

void create_driver()
{
    if (ndrivers >= MAX_DRIVERS)
    {
        printf("driver limit reached\n");
        return;
    }

    char in_path[64], out_path[64];
    snprintf(in_path, sizeof(in_path), "taxi_%d_in", ndrivers);
    snprintf(out_path, sizeof(out_path), "taxi_%d_out", ndrivers);

    mkfifo(in_path, 0666);
    mkfifo(out_path, 0666);

    pid_t pid = fork();
    if (pid == 0)
    {
        driver_main(in_path, out_path);
        exit(0);
    }

    int in_fd = open(in_path, O_RDWR);
    int out_fd = open(out_path, O_RDWR);

    drivers[ndrivers].pid = pid;
    drivers[ndrivers].in_fd = in_fd;
    drivers[ndrivers].out_fd = out_fd;
    ndrivers++;

    printf("driver created, pid=%d\n", pid);
}

void send_task(int pid, int timer)
{
    int idx = find_driver(pid);
    if (idx < 0)
    {
        printf("no such driver\n");
        return;
    }

    char cmd[64];
    snprintf(cmd, sizeof(cmd), "TASK %d\n", timer);
    write(drivers[idx].in_fd, cmd, strlen(cmd));

    char reply[64];
    if (read_line(drivers[idx].out_fd, reply, sizeof(reply)) < 0)
    {
        printf("driver not responding\n");
        return;
    }

    if (strncmp(reply, "Busy", 4) == 0) printf("%s\n", reply);
    else printf("task sent\n");
}

void get_status(int pid)
{
    int idx = find_driver(pid);
    if (idx < 0)
    {
        printf("no such driver\n");
        return;
    }

    write(drivers[idx].in_fd, "STATUS\n", 7);
    char reply[64];
    if (read_line(drivers[idx].out_fd, reply, sizeof(reply)) < 0)
    {
        printf("driver not responding\n");
        return;
    }

    printf("%d: %s\n", pid, reply);
}

void get_drivers()
{
    for (int i = 0; i < ndrivers; i++)
    {
        write(drivers[i].in_fd, "STATUS\n", 7);
        char reply[64];
        if (read_line(drivers[i].out_fd, reply, sizeof(reply)) < 0)
        {
            printf("%d: not responding\n", drivers[i].pid);
            continue;
        }
        printf("%d: %s\n", drivers[i].pid, reply);
    }
}

void shutdown_all_drivers()
{
    for (int i = 0; i < ndrivers; i++)
    {
        write(drivers[i].in_fd, "STOP\n", 5);
    }
    for (int i = 0; i < ndrivers; i++)
    {
        waitpid(drivers[i].pid, NULL, 0);
        close(drivers[i].in_fd);
        close(drivers[i].out_fd);
    }
    printf("all drivers stopped\n");
}

int main()
{
    install_signal_handlers();

    char line[128];

    while (!g_stop)
    {
        printf("> ");
        fflush(stdout);
        if (fgets(line, sizeof(line), stdin) == NULL) break;
        line[strcspn(line, "\n")] = '\0';

        int pid_arg, timer_arg;

        if (strcmp(line, "create_driver") == 0)
        {
            create_driver();
        }
        else if (sscanf(line, "send_task %d %d", &pid_arg, &timer_arg) == 2)
        {
            send_task(pid_arg, timer_arg);
        }
        else if (sscanf(line, "get_status %d", &pid_arg) == 1)
        {
            get_status(pid_arg);
        }
        else if (strcmp(line, "get_drivers") == 0)
        {
            get_drivers();
        }
        else if (strlen(line) > 0)
        {
            printf("unknown command\n");
        }
    }

    shutdown_all_drivers();
    return 0;
}
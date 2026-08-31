#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345
#define DEFAULT_BROADCAST_ADDR "255.255.255.255"

static volatile sig_atomic_t g_stop = 0;
static const char* g_broadcast_addr = DEFAULT_BROADCAST_ADDR;

void handle_stop_signal(int sig)
{
    g_stop = 1;
}

void install_signal_handlers()
{
    struct sigaction sa = { .sa_handler = handle_stop_signal };
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

int create_socket()
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    int yes = 1;
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_addr.s_addr = htonl(INADDR_ANY), .sin_port = htons(PORT) };
    bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    return fd;
}

void send_message(int fd, const char* msg)
{
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(PORT) };
    inet_pton(AF_INET, g_broadcast_addr, &addr.sin_addr);
    sendto(fd, msg, strlen(msg) + 1, 0, (struct sockaddr*)&addr, sizeof(addr));
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("usage: %s <nickname> [broadcast address]\n", argv[0]);
        return 1;
    }

    if (argc >= 3) g_broadcast_addr = argv[2];

    install_signal_handlers();

    char* nick = argv[1];
    int fd = create_socket();

    char join_msg[128];
    snprintf(join_msg, sizeof(join_msg), "%s joined the chat", nick);
    send_message(fd, join_msg);

    while (!g_stop)
    {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(fd, &set);
        FD_SET(STDIN_FILENO, &set);
        int maxfd = fd > STDIN_FILENO ? fd : STDIN_FILENO;

        int r = select(maxfd + 1, &set, NULL, NULL, NULL);
        if (r < 0)
        {
            if (errno == EINTR) continue;
            break;
        }

        if (FD_ISSET(fd, &set))
        {
            char buf[256];
            struct sockaddr_in from;
            socklen_t fromlen = sizeof(from);
            ssize_t n = recvfrom(fd, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&from, &fromlen);
            if (n > 0)
            {
                buf[n] = '\0';
                if (strncmp(buf, nick, strlen(nick)) != 0)
                {
                    printf("%s\n", buf);
                    fflush(stdout);
                }
            }
        }

        if (FD_ISSET(STDIN_FILENO, &set))
        {
            char line[200];
            if (fgets(line, sizeof(line), stdin) == NULL) break;
            line[strcspn(line, "\n")] = '\0';

            char out[256];
            snprintf(out, sizeof(out), "%s: %s", nick, line);
            send_message(fd, out);
        }
    }

    char leave_msg[128];
    snprintf(leave_msg, sizeof(leave_msg), "%s left the chat", nick);
    send_message(fd, leave_msg);

    close(fd);
    return 0;
}
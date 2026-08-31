#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 6000
#define NAME_LEN 128
#define MAX_PAYLOAD (1 << 20)

static volatile sig_atomic_t g_stop = 0;
static char g_recv_buf[MAX_PAYLOAD];
static char g_send_buf[MAX_PAYLOAD];

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

int read_n(int fd, void* buf, size_t n)
{
    size_t got = 0;
    while (got < n)
    {
        ssize_t r = read(fd, (char*)buf + got, n - got);
        if (r <= 0) return -1;
        got += r;
    }
    return 0;
}

int write_n(int fd, const void* buf, size_t n)
{
    size_t sent = 0;
    while (sent < n)
    {
        ssize_t w = write(fd, (const char*)buf + sent, n - sent);
        if (w <= 0) return -1;
        sent += w;
    }
    return 0;
}

int recv_frame(int fd, char* type, char* payload, uint32_t* len)
{
    unsigned char header[5];
    if (read_n(fd, header, 5) < 0) return -1;
    *type = (char)header[0];
    uint32_t net_len;
    memcpy(&net_len, header + 1, 4);
    *len = ntohl(net_len);
    if (*len > MAX_PAYLOAD) return -1;
    if (*len > 0 && read_n(fd, payload, *len) < 0) return -1;
    return 0;
}

int send_frame(int fd, char type, const char* payload, uint32_t len)
{
    unsigned char header[5];
    header[0] = (unsigned char)type;
    uint32_t net_len = htonl(len);
    memcpy(header + 1, &net_len, 4);
    if (write_n(fd, header, 5) < 0) return -1;
    if (len > 0 && write_n(fd, payload, len) < 0) return -1;
    return 0;
}

int connect_to_server(const char* host)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(PORT) };
    inet_pton(AF_INET, host, &addr.sin_addr);
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(1);
    }
    return fd;
}

void send_file(int fd, const char* path)
{
    FILE* f = fopen(path, "rb");
    if (!f)
    {
        printf("can't open %s\n", path);
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size < 0 || size > MAX_PAYLOAD - NAME_LEN)
    {
        printf("file too big\n");
        fclose(f);
        return;
    }

    const char* base = strrchr(path, '/');
    base = base ? base + 1 : path;

    memset(g_send_buf, 0, NAME_LEN);
    strncpy(g_send_buf, base, NAME_LEN - 1);
    fread(g_send_buf + NAME_LEN, 1, size, f);
    fclose(f);

    send_frame(fd, 'F', g_send_buf, NAME_LEN + size);
    printf("sent file %s (%ld bytes)\n", base, size);
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("usage: %s <nickname> <server ip>\n", argv[0]);
        return 1;
    }

    install_signal_handlers();

    char* nick = argv[1];
    int fd = connect_to_server(argv[2]);
    send_frame(fd, 'T', nick, strlen(nick));

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
            char type;
            uint32_t len;
            if (recv_frame(fd, &type, g_recv_buf, &len) < 0) break;

            if (type == 'T')
            {
                g_recv_buf[len] = '\0';
                printf("%s\n", g_recv_buf);
                fflush(stdout);
            }
            else if (type == 'F')
            {
                char filename[NAME_LEN];
                memcpy(filename, g_recv_buf, NAME_LEN);
                filename[NAME_LEN - 1] = '\0';
                uint32_t data_len = len - NAME_LEN;

                char outname[200];
                snprintf(outname, sizeof(outname), "recv_%s", filename);
                FILE* out = fopen(outname, "wb");
                fwrite(g_recv_buf + NAME_LEN, 1, data_len, out);
                fclose(out);

                printf("received file %s (%u bytes) -> %s\n", filename, data_len, outname);
                fflush(stdout);
            }
        }

        if (FD_ISSET(STDIN_FILENO, &set))
        {
            char line[512];
            if (fgets(line, sizeof(line), stdin) == NULL) break;
            line[strcspn(line, "\n")] = '\0';

            if (strncmp(line, "/file ", 6) == 0)
            {
                send_file(fd, line + 6);
            }
            else
            {
                char out[600];
                int n = snprintf(out, sizeof(out), "%s: %s", nick, line);
                send_frame(fd, 'T', out, n);
            }
        }
    }

    close(fd);
    return 0;
}
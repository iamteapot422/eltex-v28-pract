#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 6000
#define MAX_CLIENTS 32
#define MAX_PAYLOAD (1 << 20)

typedef struct
{
    int fd;
    char nick[64];
} client_t;

static client_t clients[MAX_CLIENTS];
static int nclients = 0;
static char g_buf[MAX_PAYLOAD];

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

void broadcast(char type, const char* payload, uint32_t len, int except_fd)
{
    for (int i = 0; i < nclients; i++)
    {
        if (clients[i].fd == except_fd) continue;
        send_frame(clients[i].fd, type, payload, len);
    }
}

void remove_client(int idx)
{
    close(clients[idx].fd);
    clients[idx] = clients[nclients - 1];
    nclients--;
}

int main()
{
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_addr.s_addr = htonl(INADDR_ANY), .sin_port = htons(PORT) };
    bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(listen_fd, 16);

    printf("server: listening on port %d\n", PORT);
    fflush(stdout);

    while (1)
    {
        struct pollfd pfds[MAX_CLIENTS + 1];
        pfds[0].fd = listen_fd;
        pfds[0].events = POLLIN;
        for (int i = 0; i < nclients; i++)
        {
            pfds[i + 1].fd = clients[i].fd;
            pfds[i + 1].events = POLLIN;
        }

        int r = poll(pfds, nclients + 1, -1);
        if (r < 0)
        {
            if (errno == EINTR) continue;
            break;
        }

        if (pfds[0].revents & POLLIN)
        {
            int cfd = accept(listen_fd, NULL, NULL);
            if (cfd >= 0)
            {
                char type;
                uint32_t len;
                if (nclients < MAX_CLIENTS && recv_frame(cfd, &type, g_buf, &len) == 0 && type == 'T')
                {
                    g_buf[len] = '\0';
                    clients[nclients].fd = cfd;
                    strncpy(clients[nclients].nick, g_buf, sizeof(clients[nclients].nick) - 1);
                    nclients++;

                    char msg[200];
                    int mlen = snprintf(msg, sizeof(msg), "%s joined the chat", g_buf);
                    broadcast('T', msg, mlen, cfd);
                    printf("server: %s connected\n", g_buf);
                    fflush(stdout);
                }
                else
                {
                    close(cfd);
                }
            }
        }

        for (int i = 0; i < nclients; i++)
        {
            if (!(pfds[i + 1].revents & (POLLIN | POLLHUP | POLLERR))) continue;

            char type;
            uint32_t len;
            if (recv_frame(clients[i].fd, &type, g_buf, &len) < 0)
            {
                char msg[200];
                int mlen = snprintf(msg, sizeof(msg), "%s left the chat", clients[i].nick);
                printf("server: %s\n", msg);
                fflush(stdout);
                remove_client(i);
                broadcast('T', msg, mlen, -1);
                continue;
            }

            broadcast(type, g_buf, len, clients[i].fd);
        }
    }

    close(listen_fd);
    return 0;
}
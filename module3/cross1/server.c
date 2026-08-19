#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 9000
#define MAX_CLIENTS 64

typedef struct
{
    struct in_addr ip;
    uint16_t port;
    int counter;
} client_entry;

client_entry clients[MAX_CLIENTS];
int nclients = 0;

int find_client(struct sockaddr_in* addr)
{
    for (int i = 0; i < nclients; i++)
    {
        if (clients[i].ip.s_addr == addr->sin_addr.s_addr && clients[i].port == addr->sin_port)
        {
            return i;
        }
    }
    return -1;
}

void remove_client(int idx)
{
    clients[idx] = clients[nclients - 1];
    nclients--;
}

int main()
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_addr.s_addr = htonl(INADDR_ANY), .sin_port = htons(PORT) };
    bind(fd, (struct sockaddr*)&addr, sizeof(addr));

    printf("server: listening on udp port %d\n", PORT);
    fflush(stdout);

    char buf[1024];

    while (1)
    {
        struct sockaddr_in from;
        socklen_t fromlen = sizeof(from);
        ssize_t n = recvfrom(fd, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&from, &fromlen);
        if (n <= 0) continue;
        buf[n] = '\0';

        int idx = find_client(&from);

        if (strcmp(buf, "CLOSE") == 0)
        {
            if (idx != -1)
            {
                printf("server: %s:%d closed, counter reset\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port));
                fflush(stdout);
                remove_client(idx);
            }
            continue;
        }

        if (idx == -1)
        {
            if (nclients >= MAX_CLIENTS) continue;
            idx = nclients++;
            clients[idx].ip = from.sin_addr;
            clients[idx].port = from.sin_port;
            clients[idx].counter = 0;
        }

        clients[idx].counter++;

        char reply[1100];
        int len = snprintf(reply, sizeof(reply), "%s %d", buf, clients[idx].counter);
        sendto(fd, reply, len, 0, (struct sockaddr*)&from, fromlen);

        printf("server: %s:%d -> \"%s\"\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port), reply);
        fflush(stdout);
    }

    close(fd);
    return 0;
}
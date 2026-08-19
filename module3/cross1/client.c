#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 9000

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
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("usage: %s <server ip>\n", argv[0]);
        return 1;
    }

    install_signal_handlers();

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server = { .sin_family = AF_INET, .sin_port = htons(PORT) };
    inet_pton(AF_INET, argv[1], &server.sin_addr);

    char line[1024];

    while (!g_stop)
    {
        printf("> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;

        sendto(fd, line, strlen(line), 0, (struct sockaddr*)&server, sizeof(server));

        char reply[1100];
        ssize_t n = recvfrom(fd, reply, sizeof(reply) - 1, 0, NULL, NULL);
        if (n > 0)
        {
            reply[n] = '\0';
            printf("%s\n", reply);
        }
    }

    sendto(fd, "CLOSE", 5, 0, (struct sockaddr*)&server, sizeof(server));
    close(fd);
    return 0;
}
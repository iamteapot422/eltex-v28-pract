#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define PORT 7000
#define BUF_SIZE 2048

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

unsigned int checksum_add(unsigned int sum, void* data, int len)
{
    unsigned short* buf = data;
    for (; len > 1; len -= 2) sum += *buf++;
    if (len == 1) sum += *(unsigned char*)buf;
    return sum;
}

unsigned short checksum_done(unsigned int sum)
{
    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    return (unsigned short)~sum;
}

struct pseudo_header
{
    uint32_t src;
    uint32_t dst;
    uint8_t zero;
    uint8_t protocol;
    uint16_t len;
};

int build_packet(char* buf, uint32_t src_ip, uint16_t src_port, uint32_t dst_ip, uint16_t dst_port, const char* payload, int payload_len)
{
    struct iphdr* ip = (struct iphdr*)buf;
    struct udphdr* udp = (struct udphdr*)(buf + sizeof(struct iphdr));
    int udp_len = sizeof(struct udphdr) + payload_len;

    memcpy(buf + sizeof(struct iphdr) + sizeof(struct udphdr), payload, payload_len);

    udp->source = htons(src_port);
    udp->dest = htons(dst_port);
    udp->len = htons(udp_len);
    udp->check = 0;

    struct pseudo_header ph = { src_ip, dst_ip, 0, IPPROTO_UDP, udp->len };
    unsigned int sum = checksum_add(0, &ph, sizeof(ph));
    sum = checksum_add(sum, udp, udp_len);
    udp->check = checksum_done(sum);

    *ip = (struct iphdr){ .ihl = 5, .version = 4, .ttl = 64, .protocol = IPPROTO_UDP,
                           .tot_len = htons(sizeof(struct iphdr) + udp_len),
                           .saddr = src_ip, .daddr = dst_ip };
    ip->check = checksum_done(checksum_add(0, ip, sizeof(struct iphdr)));

    return sizeof(struct iphdr) + udp_len;
}

void send_payload(int sock, uint32_t own_ip, uint16_t own_port, uint32_t server_ip, char type, const char* text, int text_len)
{
    char payload[BUF_SIZE];
    payload[0] = type;
    if (text_len > 0) memcpy(payload + 1, text, text_len);

    char packet[BUF_SIZE];
    int plen = build_packet(packet, own_ip, own_port, server_ip, PORT, payload, 1 + text_len);

    struct sockaddr_in dst = { .sin_family = AF_INET, .sin_addr.s_addr = server_ip };
    sendto(sock, packet, plen, 0, (struct sockaddr*)&dst, sizeof(dst));
}

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        printf("usage: %s <own ip> <server ip> <own port>\n", argv[0]);
        return 1;
    }

    install_signal_handlers();

    uint32_t own_ip, server_ip;
    inet_pton(AF_INET, argv[1], &own_ip);
    inet_pton(AF_INET, argv[2], &server_ip);
    uint16_t own_port = (uint16_t)atoi(argv[3]);

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock < 0)
    {
        perror("socket (run as root?)");
        return 1;
    }

    int one = 1;
    setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one));

    char buf[BUF_SIZE];

    while (!g_stop)
    {
        char line[256];
        if (fgets(line, sizeof(line), stdin) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;

        send_payload(sock, own_ip, own_port, server_ip, 'M', line, strlen(line));

        while (!g_stop)
        {
            ssize_t n = recvfrom(sock, buf, sizeof(buf), 0, NULL, NULL);
            if (n < 0)
            {
                if (errno == EINTR) break;
                break;
            }
            if (n < (ssize_t)sizeof(struct iphdr)) continue;

            struct iphdr* ip = (struct iphdr*)buf;
            int ip_hlen = ip->ihl * 4;
            if (n < ip_hlen + (ssize_t)sizeof(struct udphdr)) continue;

            struct udphdr* udp = (struct udphdr*)(buf + ip_hlen);
            if (ntohs(udp->dest) != own_port || ip->saddr != server_ip) continue;

            int udp_len = ntohs(udp->len);
            int payload_len = udp_len - sizeof(struct udphdr);
            if (payload_len <= 0 || ip_hlen + udp_len > n) continue;

            char* payload = buf + ip_hlen + sizeof(struct udphdr);
            printf("server: %.*s\n", payload_len, payload);
            fflush(stdout);
            break;
        }
    }

    send_payload(sock, own_ip, own_port, server_ip, 'C', NULL, 0);
    printf("client: sent close notification\n");

    close(sock);
    return 0;
}
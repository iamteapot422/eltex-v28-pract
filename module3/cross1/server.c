#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define PORT 7000
#define BUF_SIZE 2048
#define MAX_CLIENTS 64

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

typedef struct
{
    uint32_t ip;
    uint16_t port;
    int counter;
} client_state_t;

static client_state_t clients[MAX_CLIENTS];
static int nclients = 0;

int find_client(uint32_t ip, uint16_t port)
{
    for (int i = 0; i < nclients; i++)
    {
        if (clients[i].ip == ip && clients[i].port == port) return i;
    }
    return -1;
}

void remove_client(int idx)
{
    clients[idx] = clients[nclients - 1];
    nclients--;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("usage: %s <own ip>\n", argv[0]);
        return 1;
    }

    uint32_t own_ip;
    inet_pton(AF_INET, argv[1], &own_ip);

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock < 0)
    {
        perror("socket (run as root?)");
        return 1;
    }

    int one = 1;
    setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one));

    printf("server: listening on udp port %d\n", PORT);
    fflush(stdout);

    char buf[BUF_SIZE];
    char packet[BUF_SIZE];

    while (1)
    {
        ssize_t n = recvfrom(sock, buf, sizeof(buf), 0, NULL, NULL);
        if (n < (ssize_t)sizeof(struct iphdr)) continue;

        struct iphdr* ip = (struct iphdr*)buf;
        int ip_hlen = ip->ihl * 4;
        if (n < ip_hlen + (ssize_t)sizeof(struct udphdr)) continue;

        struct udphdr* udp = (struct udphdr*)(buf + ip_hlen);
        if (ntohs(udp->dest) != PORT) continue;

        int udp_len = ntohs(udp->len);
        int payload_len = udp_len - sizeof(struct udphdr);
        if (payload_len < 1 || ip_hlen + udp_len > n) continue;

        char* payload = buf + ip_hlen + sizeof(struct udphdr);
        char type = payload[0];
        char* text = payload + 1;
        int text_len = payload_len - 1;

        uint32_t cip = ip->saddr;
        uint16_t cport = ntohs(udp->source);

        struct in_addr a;
        a.s_addr = cip;

        if (type == 'C')
        {
            int idx = find_client(cip, cport);
            if (idx >= 0)
            {
                remove_client(idx);
                printf("server: %s:%d closed, counter reset\n", inet_ntoa(a), cport);
                fflush(stdout);
            }
            continue;
        }

        if (type != 'M') continue;

        int idx = find_client(cip, cport);
        if (idx < 0)
        {
            if (nclients >= MAX_CLIENTS) continue;
            idx = nclients++;
            clients[idx].ip = cip;
            clients[idx].port = cport;
            clients[idx].counter = 0;
        }
        clients[idx].counter++;

        char reply[BUF_SIZE];
        int reply_len = snprintf(reply, sizeof(reply), "%.*s %d", text_len, text, clients[idx].counter);

        printf("server: %s:%d: %.*s -> \"%s\"\n", inet_ntoa(a), cport, text_len, text, reply);
        fflush(stdout);

        int plen = build_packet(packet, own_ip, PORT, cip, cport, reply, reply_len);
        struct sockaddr_in dst = { .sin_family = AF_INET, .sin_addr.s_addr = cip };
        sendto(sock, packet, plen, 0, (struct sockaddr*)&dst, sizeof(dst));
    }

    close(sock);
    return 0;
}
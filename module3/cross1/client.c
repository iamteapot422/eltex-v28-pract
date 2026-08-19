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
#define CLIENT_PORT 8000
#define BUF_SIZE 2048

unsigned short checksum(void* data, int len)
{
    unsigned short* buf = data;
    unsigned int sum = 0;
    for (; len > 1; len -= 2) sum += *buf++;
    if (len == 1) sum += *(unsigned char*)buf;
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
    char* data = buf + sizeof(struct iphdr) + sizeof(struct udphdr);
    memcpy(data, payload, payload_len);

    udp->source = htons(src_port);
    udp->dest = htons(dst_port);
    udp->len = htons(sizeof(struct udphdr) + payload_len);
    udp->check = 0;

    struct pseudo_header ph = { src_ip, dst_ip, 0, IPPROTO_UDP, udp->len };
    int plen = sizeof(ph) + sizeof(struct udphdr) + payload_len;
    char* pbuf = malloc(plen);
    memcpy(pbuf, &ph, sizeof(ph));
    memcpy(pbuf + sizeof(ph), udp, sizeof(struct udphdr) + payload_len);
    udp->check = checksum(pbuf, plen);
    free(pbuf);

    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + payload_len);
    ip->id = htons(rand() % 65536);
    ip->frag_off = 0;
    ip->ttl = 64;
    ip->protocol = IPPROTO_UDP;
    ip->check = 0;
    ip->saddr = src_ip;
    ip->daddr = dst_ip;
    ip->check = checksum(ip, sizeof(struct iphdr));

    return sizeof(struct iphdr) + sizeof(struct udphdr) + payload_len;
}

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        printf("usage: %s <own ip> <server ip> <message>\n", argv[0]);
        return 1;
    }

    uint32_t own_ip, server_ip;
    inet_pton(AF_INET, argv[1], &own_ip);
    inet_pton(AF_INET, argv[2], &server_ip);
    const char* message = argv[3];

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock < 0)
    {
        perror("socket (run as root?)");
        return 1;
    }

    int one = 1;
    setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one));

    char packet[BUF_SIZE];
    int plen = build_packet(packet, own_ip, CLIENT_PORT, server_ip, PORT, message, strlen(message));

    struct sockaddr_in dst = { .sin_family = AF_INET, .sin_addr.s_addr = server_ip };
    sendto(sock, packet, plen, 0, (struct sockaddr*)&dst, sizeof(dst));
    printf("client: sent \"%s\"\n", message);
    fflush(stdout);

    char buf[BUF_SIZE];
    while (1)
    {
        struct sockaddr_in from;
        socklen_t fromlen = sizeof(from);
        ssize_t n = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&from, &fromlen);
        if (n < (ssize_t)sizeof(struct iphdr)) continue;

        struct iphdr* ip = (struct iphdr*)buf;
        int ip_hlen = ip->ihl * 4;
        if (n < ip_hlen + (ssize_t)sizeof(struct udphdr)) continue;

        struct udphdr* udp = (struct udphdr*)(buf + ip_hlen);
        if (ntohs(udp->dest) != CLIENT_PORT) continue;
        if (ip->saddr != server_ip) continue;

        int udp_len = ntohs(udp->len);
        int payload_len = udp_len - sizeof(struct udphdr);
        if (payload_len <= 0 || ip_hlen + udp_len > n) continue;

        char* payload = buf + ip_hlen + sizeof(struct udphdr);
        printf("client: echo: %.*s\n", payload_len, payload);
        fflush(stdout);
        break;
    }

    close(sock);
    return 0;
}
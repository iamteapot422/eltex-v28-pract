#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>

#define CHAT_PORT 12345
#define DNS_PORT 53

volatile sig_atomic_t stop_flag = 0;

void on_signal(int sig) { (void)sig; stop_flag = 1; }

void mac_str(const unsigned char *m, char *out)
{
    sprintf(out, "%02X:%02X:%02X:%02X:%02X:%02X", m[0], m[1], m[2], m[3], m[4], m[5]);
}

int match_filter(int mode, int sport, int dport)
{
    if (mode == 0) return 1;
    if (mode == 1) return sport == CHAT_PORT || dport == CHAT_PORT;
    if (mode == 2) return sport == DNS_PORT || dport == DNS_PORT;
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("usage: sudo %s <interface>\n", argv[0]);
        return 1;
    }

    signal(SIGINT, on_signal);

    int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    if (fd < 0)
    {
        perror("socket");
        return 1;
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, argv[1], IFNAMSIZ - 1);
    ioctl(fd, SIOCGIFINDEX, &ifr);

    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifr.ifr_ifindex;
    sll.sll_protocol = htons(ETH_P_IP);
    bind(fd, (struct sockaddr *)&sll, sizeof(sll));

    printf("Filter: 1 - chat (port %d), 2 - DNS (port %d), 0 - no filter\n", CHAT_PORT, DNS_PORT);
    printf("Select: ");
    int filter_mode;
    scanf("%d", &filter_mode);

    printf("Filename: ");
    char filename[256];
    scanf("%255s", filename);

    printf("Capturing...\n");

    FILE *f = fopen(filename, "w");

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    unsigned char buf[65536];

    while (!stop_flag)
    {
        ssize_t n = recv(fd, buf, sizeof(buf), 0);
        if (n < 0)
        {
            if (errno == EINTR) continue;
            break;
        }
        if ((size_t)n < sizeof(struct ethhdr)) continue;

        struct ethhdr *eth = (struct ethhdr *)buf;
        if (ntohs(eth->h_proto) != ETH_P_IP) continue;

        struct iphdr *iph = (struct iphdr *)(buf + sizeof(struct ethhdr));
        if (iph->protocol != IPPROTO_UDP) continue;

        int ip_len = iph->ihl * 4;
        struct udphdr *udph = (struct udphdr *)(buf + sizeof(struct ethhdr) + ip_len);

        int sport = ntohs(udph->source);
        int dport = ntohs(udph->dest);
        if (!match_filter(filter_mode, sport, dport)) continue;

        clock_gettime(CLOCK_MONOTONIC, &t1);
        double t = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

        char mac_src[18], mac_dst[18];
        mac_str(eth->h_source, mac_src);
        mac_str(eth->h_dest, mac_dst);

        char ip_src[16], ip_dst[16];
        struct in_addr a;
        a.s_addr = iph->saddr;
        strcpy(ip_src, inet_ntoa(a));
        a.s_addr = iph->daddr;
        strcpy(ip_dst, inet_ntoa(a));

        char line[512];
        snprintf(line, sizeof(line),
            "t=+%.3f  MAC %s -> %s  IP %s:%d -> %s:%d\n",
            t, mac_src, mac_dst, ip_src, sport, ip_dst, dport);

        printf("%s", line);
        if (f) fprintf(f, "%s", line);
    }

    if (f) fclose(f);
    close(fd);
    return 0;
}
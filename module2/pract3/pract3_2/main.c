#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

void displaybitbybit(uint32_t mask)
{
    for (int i = 31; i >= 0; i--)
    {
        printf("%d", (mask >> i) & 1);
    }
    printf("\n");
}
int main() {
    char ipstr[16];
    uint32_t ipaddr = 0;
    uint32_t mask;
    int count;

    scanf_s("%15[^/]/%d %d", ipstr, (unsigned)sizeof(ipstr), &mask, &count);

    mask = ~((1 << (32 - mask)) - 1);
    printf("Mask: ");
    displaybitbybit(mask);

    char* token = strtok(ipstr, ".");
    for (int i = 0; i < 4 && token; i++) {
        ipaddr += atoi(token) << (i * 8);
        token = strtok(NULL, ".");
    }
    displaybitbybit(ipaddr);
    uint32_t network = ipaddr & mask;
    printf("Network: ");
    displaybitbybit(network);
    srand(time(NULL));
    int right = 0;
    for (int i = 0; i < count; i++)
    {
        uint32_t raddr = ((uint32_t)rand() << 18) + ((uint32_t)rand() << 2);
        if ((raddr & mask) == network)
            right++;
    }
    printf("Right network: %d, %.2f%%\n", right, (double)right / count * 100.0);
}


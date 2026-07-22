#include <stdio.h>
#include "PriorityQueue.h"

int main(void) {
    PriorityQueue pq = createPriorityQueue();
    insert(&pq, 3, "fish");
    insert(&pq, 5, "fishfish");
    insert(&pq, 4, "fishfishfish");
    insert(&pq, 2, "fishfishfishfish");

    return 0;
}

#include <stdio.h>
#include "PriorityQueue.h"

int main(void) {
    PriorityQueue pq = createPriorityQueue();
    insert(&pq, 3, "fish");
    insert(&pq, 5, "fishfish");
    insert(&pq, 4, "fishfishfish");
    insert(&pq, 2, "fishfishfishfish");
    remove_entry(&pq, 3, "fish");
    remove_entry(&pq, 5, NULL);
    printf("%s\n", popEntry(&pq, 2, NULL)->data);
    printf("%s\n", getWithPriority(&pq, 4)->data);
    insert(&pq, 7, "fish2");
    printf("%s\n", getWithPriorityAtLeast(&pq, 6)->data);
    return 0;
}

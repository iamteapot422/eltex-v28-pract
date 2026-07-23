#include <stdio.h>
#include "PriorityQueue.h"
#include "tests.h"

int main(void) {
    run_tests();
    printf("\n\n\n\n");
    PriorityQueue pq = createPriorityQueue();
    insert(&pq, 3, "fish");
    printPriorityQueue(&pq);
    insert(&pq, 5, "fishfish");
    printPriorityQueue(&pq);
    insert(&pq, 4, "fishfishfish");
    printPriorityQueue(&pq);
    insert(&pq, 2, "fishfishfishfish");
    printPriorityQueue(&pq);
    remove_entry(&pq, 3, "fish");
    printPriorityQueue(&pq);
    remove_entry(&pq, 5, NULL);
    printPriorityQueue(&pq);
    printf("%s\n", popEntry(&pq, 2, NULL)->data);
    printPriorityQueue(&pq);
    printf("%s\n", getWithPriority(&pq, 4)->data);
    insert(&pq, 7, "fish2");
    printf("%s\n", getWithPriorityAtLeast(&pq, 6)->data);
    return 0;
}

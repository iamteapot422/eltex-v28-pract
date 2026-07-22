#include <stdio.h>
#include <stdlib.h>
//
// Created by iamteapot on 22.07.2026.
//
typedef struct PriorityQueueEntry {
    int priority;
    struct PriorityQueueEntry *next;
    char* data;
} PriorityQueueEntry;
typedef struct {
    int size;
    PriorityQueueEntry *first;
    PriorityQueueEntry *last;
} PriorityQueue;
PriorityQueue createPriorityQueue() {
    PriorityQueue pq;
    pq.first = NULL;
    pq.last = NULL;
    pq.size = 0;
    return pq;
}
void printPriorityQueue(PriorityQueue* pq) {
    PriorityQueueEntry* entry = pq->first;
    while (entry != NULL) {
        printf("%d %s -> ", entry->priority, entry->data);
        entry = entry->next;
    }
    printf("\n");
}
PriorityQueueEntry createQueueEntry(int priority, char* data) {
    PriorityQueueEntry entry;
    entry.priority = priority;
    entry.data = data;
    entry.next = NULL;
    return entry;
}

PriorityQueueEntry* insert(PriorityQueue* pq, int priority, char* data) {
    PriorityQueueEntry* entry = malloc(sizeof(PriorityQueueEntry));
    entry->priority = priority;
    entry->data = data;
    entry->next = NULL;
    if (pq->first == NULL) {
        pq->first = entry;
        pq->last = entry;
    }
    else {
        PriorityQueueEntry** prev = &(pq->first);
        for (int i = 0; i < pq->size + 1; i++) {
            if (*prev == NULL) {
                *prev = entry;
                pq->last = entry;
                break;
            }
            if ((*prev)->priority > entry->priority) {
                entry->next = (*prev)->next;
                *prev = entry;
                break;
            }
            prev = &((*prev)->next);
        }
    }
    pq->size++;
    printPriorityQueue(pq);
    return entry;
}
int remove_entry(PriorityQueue* pq, int priority, char* data) {
    if (data != NULL) {
        PriorityQueueEntry* entry = pq->first;
        for (int i = 0; i < pq->size + 1; i++) {
        }

    }
}
/*PriorityQueueEntry* getFirst(PriorityQueue* pq);
PriorityQueueEntry* getWithPriority(PriorityQueue* pq, int priority);
PriorityQueueEntry* getWithPriorityAtLeast(PriorityQueue* pq, int priority);*/

void destroyPriorityQueue(PriorityQueue* pq);
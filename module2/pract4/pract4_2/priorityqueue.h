//
// Created by iamteapot on 22.07.2026.
//

#ifndef PRACT4_2_PRIORITYQUEUE_H
#define PRACT4_2_PRIORITYQUEUE_H
typedef struct PriorityQueueEntry {
    int priority;
    struct PriorityQueueEntry *next;
    char* data;
} PriorityQueueEntry;
typedef struct {
    int size;
    PriorityQueueEntry *first;
} PriorityQueue;
PriorityQueue createPriorityQueue();
PriorityQueueEntry* insert(PriorityQueue* pq, int priority, char* data);
int remove_entry(PriorityQueue* pq, int priority, char* data);
PriorityQueueEntry* popEntry(PriorityQueue* pq, int priority, char* data);
PriorityQueueEntry* getFirst(PriorityQueue* pq);
PriorityQueueEntry* getWithPriority(PriorityQueue* pq, int priority);
PriorityQueueEntry* getWithPriorityAtLeast(PriorityQueue* pq, int priority);
void printPriorityQueue(PriorityQueue* pq);
void destroyPriorityQueue(PriorityQueue* pq);
#endif //PRACT4_2_PRIORITYQUEUE_H

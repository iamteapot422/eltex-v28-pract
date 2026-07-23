#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priorityqueue.h"
//
// Created by iamteapot on 22.07.2026.
//
PriorityQueue createPriorityQueue() {
    PriorityQueue pq;
    pq.first = NULL;
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
    PriorityQueueEntry** prev = &(pq->first);
    for (int i = 0; i < pq->size + 1; i++) {
        if (*prev == NULL || (*prev)->priority > entry->priority) {
            entry->next = *prev;
            *prev = entry;
            break;
        }
        prev = &(*prev)->next;
    }
    pq->size++;
    return entry;
}
int remove_entry(PriorityQueue* pq, int priority, char* data) {
    PriorityQueueEntry** entry = &pq->first;
    if (data != NULL) {
        while (*entry != NULL) {
            if ((*entry)->priority == priority && strcmp((*entry)->data, data) == 0) {
                break;
            }
            entry = &(*entry)->next;
        }
    }
    else {
        while (*entry != NULL) {
            if ((*entry)->priority == priority) {
                break;
            }
            entry = &(*entry)->next;
        }
    }
    if (*entry != NULL) {
        PriorityQueueEntry* entryToRemove = *entry;
        *entry = entryToRemove->next;
        free(entryToRemove);
        pq->size--;
        return 1;
    }
    else {
        return 0;
    }
}
PriorityQueueEntry* popEntry(PriorityQueue* pq, int priority, char* data) {
    PriorityQueueEntry** entry = &pq->first;
    if (data != NULL) {
        while (*entry != NULL) {
            if ((*entry)->priority == priority && strcmp((*entry)->data, data) == 0) {
                break;
            }
            entry = &(*entry)->next;
        }
    }
    else {
        while (*entry != NULL) {
            if ((*entry)->priority == priority) {
                break;
            }
            entry = &(*entry)->next;
        }
    }
    if (*entry != NULL) {
        PriorityQueueEntry* entryToRemove = *entry;
        *entry = entryToRemove->next;
        pq->size--;
        return entryToRemove;
    }
    else {
        return NULL;
    }
}
PriorityQueueEntry* getFirst(PriorityQueue* pq) {
    return pq->first;
}
PriorityQueueEntry* getWithPriority(PriorityQueue* pq, int priority) {
    PriorityQueueEntry* entry = pq->first;
    while (entry != NULL) {
        if (entry->priority == priority) {
            break;
        }
        entry = entry->next;
    }
    return entry;
}
PriorityQueueEntry* getWithPriorityAtLeast(PriorityQueue* pq, int priority) {
    PriorityQueueEntry** entry = &pq->first;
    PriorityQueueEntry* entryToRet = NULL;
    while (*entry != NULL) {
        if ((*entry)->priority >= priority) {
            entryToRet = *entry;
            break;
        }
        entry = &(*entry)->next;
    }
    return entryToRet;
}
void destroyPriorityQueue(PriorityQueue* pq) {
    PriorityQueueEntry* entry = pq->first;
    pq->first = NULL;
    while (entry != NULL) {
        PriorityQueueEntry* nextEntry = entry->next;
        free(entry);
        entry = nextEntry;
    }
    free(pq);
}
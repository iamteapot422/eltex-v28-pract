#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priorityqueue.h"

static int tests_run = 0;
static int tests_failed = 0;

#define CHECK(cond, msg)                                                   \
    do {                                                                   \
        if (!(cond)) {                                                     \
            printf("    -> FAIL: %s (line %d)\n", msg, __LINE__);          \
            local_ok = 0;                                                  \
        }                                                                  \
    } while (0)

#define RUN_TEST(fn)                                                       \
    do {                                                                   \
        tests_run++;                                                       \
        printf("TEST: %s\n", #fn);                                        \
        if (fn()) {                                                        \
            printf("[PASS] %s\n\n", #fn);                                 \
        } else {                                                           \
            printf("[FAIL] %s\n\n", #fn);                                 \
            tests_failed++;                                                \
        }                                                                  \
    } while (0)

static void clearQueue(PriorityQueue* pq) {
    while (pq->first != NULL) {
        PriorityQueueEntry* e = popEntry(pq, pq->first->priority, pq->first->data);
        free(e);
    }
}

static int test_create_empty_queue(void) {
    int local_ok = 1;
    PriorityQueue pq = createPriorityQueue();

    CHECK(pq.first == NULL, "a new queue must be empty (first == NULL)");
    CHECK(pq.size == 0, "a new queue's size must be 0");

    return local_ok;
}

static int test_insert_orders_by_priority(void) {
    int local_ok = 1;
    PriorityQueue pq = createPriorityQueue();

    /* Simulate messages arriving in random order */
    insert(&pq, 5, "Regular message");
    insert(&pq, 1, "Urgent: server failure");
    insert(&pq, 9, "Advertisement");
    insert(&pq, 3, "System notification");
    insert(&pq, 1, "Urgent: network outage");

    CHECK(pq.size == 5, "after 5 inserts the queue size must be 5");

    int expected[] = {1, 1, 3, 5, 9};
    PriorityQueueEntry* e = pq.first;
    for (int i = 0; i < 5; i++) {
        CHECK(e != NULL, "the queue must not end earlier than expected");
        if (e != NULL) {
            CHECK(e->priority == expected[i], "priorities must be in ascending order");
            e = e->next;
        }
    }
    CHECK(e == NULL, "there must be NULL after the last element");

    CHECK(strcmp(pq.first->data, "Urgent: server failure") == 0,
          "for equal priority the insertion order must be preserved (earlier one stays first)");

    clearQueue(&pq);
    return local_ok;
}

static int test_get_first(void) {
    int local_ok = 1;
    PriorityQueue pq = createPriorityQueue();

    CHECK(getFirst(&pq) == NULL, "getFirst on an empty queue must return NULL");

    insert(&pq, 7, "Informational message");
    insert(&pq, 2, "Authorization error");
    insert(&pq, 4, "Disk warning");

    PriorityQueueEntry* first = getFirst(&pq);
    CHECK(first != NULL, "getFirst must return a non-NULL pointer");
    if (first != NULL) {
        CHECK(first->priority == 2, "the first entry must have priority 2");
        CHECK(strcmp(first->data, "Authorization error") == 0,
              "the first entry must be exactly the authorization error message");
    }

    clearQueue(&pq);
    return local_ok;
}

static int test_get_with_exact_priority(void) {
    int local_ok = 1;
    PriorityQueue pq = createPriorityQueue();

    insert(&pq, 1, "Critical DB error");
    insert(&pq, 5, "Login info");
    insert(&pq, 5, "Logout info");
    insert(&pq, 8, "Debug log");

    PriorityQueueEntry* found = getWithPriority(&pq, 5);
    CHECK(found != NULL, "a message with priority 5 must be found");
    if (found != NULL) {
        /* since priority 5 occurs twice, the first inserted one must be returned */
        CHECK(strcmp(found->data, "Login info") == 0,
              "when several messages share a priority, the first one in the queue is returned");
    }

    PriorityQueueEntry* missing = getWithPriority(&pq, 42);
    CHECK(missing == NULL, "searching for a non-existent priority must return NULL");

    clearQueue(&pq);
    return local_ok;
}

static int test_get_with_priority_at_least(void) {
    int local_ok = 1;
    PriorityQueue pq = createPriorityQueue();

    insert(&pq, 1, "Urgent");
    insert(&pq, 3, "Important");
    insert(&pq, 6, "Normal");
    insert(&pq, 10, "Unimportant");

    PriorityQueueEntry* atLeast4 = getWithPriorityAtLeast(&pq, 4);
    CHECK(atLeast4 != NULL, "a message with priority >= 4 must be found");
    if (atLeast4 != NULL) {
        CHECK(atLeast4->priority == 6, "the first matching message must have priority 6");
    }

    PriorityQueueEntry* atLeast0 = getWithPriorityAtLeast(&pq, 0);
    CHECK(atLeast0 != NULL && atLeast0->priority == 1,
          "'at least 0' must return the message with the smallest priority (1)");

    PriorityQueueEntry* none = getWithPriorityAtLeast(&pq, 100);
    CHECK(none == NULL, "if no message matches, NULL must be returned");

    clearQueue(&pq);
    return local_ok;
}

static int test_remove_entry(void) {
    int local_ok = 1;
    PriorityQueue pq = createPriorityQueue();

    insert(&pq, 2, "Message A");
    insert(&pq, 2, "Message B");
    insert(&pq, 4, "Message C");

    CHECK(pq.size == 3, "size must be 3 before removal");

    int removed = remove_entry(&pq, 2, "Message B");
    CHECK(removed == 1, "removing an existing message must return 1");
    CHECK(pq.size == 2, "size must drop to 2 after removal");
    CHECK(getWithPriority(&pq, 2) != NULL &&
          strcmp(getWithPriority(&pq, 2)->data, "Message A") == 0,
          "the remaining priority-2 message must be 'Message A'");

    removed = remove_entry(&pq, 2, NULL);
    CHECK(removed == 1, "removing by priority only must return 1");
    CHECK(pq.size == 1, "size must be 1 after the second removal");
    CHECK(getWithPriority(&pq, 2) == NULL,
          "no priority-2 messages must remain");

    removed = remove_entry(&pq, 99, "No such message");
    CHECK(removed == 0, "removing a non-existent message must return 0");
    CHECK(pq.size == 1, "size must not change on a failed removal");

    clearQueue(&pq);
    return local_ok;
}

static int test_pop_entry_processing_order(void) {
    int local_ok = 1;
    PriorityQueue pq = createPriorityQueue();

    insert(&pq, 5, "Fifth");
    insert(&pq, 1, "First");
    insert(&pq, 3, "Third");
    insert(&pq, 1, "First-bis");
    insert(&pq, 2, "Second");

    int expectedPriorities[] = {1, 1, 2, 3, 5};
    int count = 0;

    while (pq.first != NULL) {
        int p = pq.first->priority;
        PriorityQueueEntry* popped = popEntry(&pq, p, pq.first->data);
        CHECK(popped != NULL, "popEntry must not return NULL while the queue isn't empty");
        if (popped != NULL) {
            CHECK(popped->priority == expectedPriorities[count],
                  "messages must be popped strictly in ascending priority order");
            free(popped);
        }
        count++;
    }

    CHECK(count == 5, "exactly 5 messages must have been popped");
    CHECK(pq.size == 0, "size must be 0 after popping all messages");
    CHECK(pq.first == NULL, "the queue must be empty after popping all messages");

    PriorityQueueEntry* nothing = popEntry(&pq, 1, NULL);
    CHECK(nothing == NULL, "popEntry on an empty queue must return NULL");

    return local_ok;
}

static int test_realistic_message_simulation(void) {
    int local_ok = 1;
    PriorityQueue pq = createPriorityQueue();

    insert(&pq, 0, "CRITICAL: disk failure");
    insert(&pq, 0, "CRITICAL: network loss");

    insert(&pq, 5, "You have a new message");
    insert(&pq, 5, "A friend joined the chat");

    insert(&pq, 9, "Special offer");

    CHECK(pq.size == 5, "5 messages must have been generated in total");

    PriorityQueueEntry* critical = getWithPriority(&pq, 0);
    CHECK(critical != NULL, "a critical message must be found");

    PriorityQueueEntry* important = getWithPriorityAtLeast(&pq, 3);
    CHECK(important != NULL && important->priority == 5,
          "the first message with priority >= 3 must have priority 5");

    PriorityQueueEntry* first = popEntry(&pq, pq.first->priority, pq.first->data);
    CHECK(first != NULL && first->priority == 0,
          "the first message processed must be a critical one");
    free(first);

    PriorityQueueEntry* second = popEntry(&pq, pq.first->priority, pq.first->data);
    CHECK(second != NULL && second->priority == 0,
          "the second message processed must also be critical");
    free(second);

    CHECK(getWithPriority(&pq, 0) == NULL,
          "no critical messages must remain in the queue");
    CHECK(pq.size == 3, "3 messages must remain after processing the two critical ones");

    clearQueue(&pq);
    return local_ok;
}

int run_tests() {
    RUN_TEST(test_create_empty_queue);
    RUN_TEST(test_insert_orders_by_priority);
    RUN_TEST(test_get_first);
    RUN_TEST(test_get_with_exact_priority);
    RUN_TEST(test_get_with_priority_at_least);
    RUN_TEST(test_remove_entry);
    RUN_TEST(test_pop_entry_processing_order);
    RUN_TEST(test_realistic_message_simulation);

    printf("Total tests:  %d\n", tests_run);
    printf("Failed:       %d\n", tests_failed);
    printf("Passed:       %d\n", tests_run - tests_failed);

    return tests_failed == 0 ? 0 : 1;
}
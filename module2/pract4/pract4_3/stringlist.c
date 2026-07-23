#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    char** items;
    int count;
    int capacity;
} StringList;

void stringlist_init(StringList* sl) {
    sl->items = NULL;
    sl->count = 0;
    sl->capacity = 0;
}

void stringlist_add(StringList* sl, const char* value) {
    if (sl->count == sl->capacity) {
        int new_capacity = sl->capacity == 0 ? 1 : sl->capacity * 2;
        sl->items = realloc(sl->items, new_capacity * sizeof(char*));
        sl->capacity = new_capacity;
    }
    sl->items[sl->count] = malloc(strlen(value) + 1);
    strcpy(sl->items[sl->count], value);
    sl->count++;
}

void stringlist_remove(StringList* sl, int index) {
    if (index < 0 || index >= sl->count) return;
    free(sl->items[index]);
    for (int i = index; i < sl->count - 1; i++) {
        sl->items[i] = sl->items[i + 1];
    }
    sl->count--;
}

void stringlist_free(StringList* sl) {
    for (int i = 0; i < sl->count; i++) {
        free(sl->items[i]);
    }
    free(sl->items);
    sl->items = NULL;
    sl->count = 0;
    sl->capacity = 0;
}

void stringlist_print(const StringList* sl, const char* label) {
    if (sl->count == 0) {
        printf("  %s: (not specified)\n", label);
        return;
    }
    printf("  %s:\n", label);
    for (int i = 0; i < sl->count; i++) {
        printf("    %d) %s\n", i + 1, sl->items[i]);
    }
}
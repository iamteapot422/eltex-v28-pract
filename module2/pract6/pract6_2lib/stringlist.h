
#ifndef PRACT2_1_STRINGLIST_H
#define PRACT2_1_STRINGLIST_H
typedef struct {
    char** items;
    int count;
    int capacity;
} StringList;

void stringlist_init(StringList* sl);
void stringlist_add(StringList* sl, const char* value);
void stringlist_remove(StringList* sl, int index);
void stringlist_free(StringList* sl);
void stringlist_print(const StringList* sl, const char* label);
#endif //PRACT2_1_STRINGLIST_H

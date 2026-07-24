#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "phonebook.h"

void test_stringlist_operations(void) {
    StringList sl;
    stringlist_init(&sl);

    assert(sl.count == 0);
    assert(sl.capacity == 0);
    assert(sl.items == NULL);

    stringlist_add(&sl, "+79991112233");
    stringlist_add(&sl, "+79992223344");
    
    assert(sl.count == 2);
    assert(sl.capacity >= 2);
    assert(strcmp(sl.items[0], "+79991112233") == 0);
    assert(strcmp(sl.items[1], "+79992223344") == 0);

    stringlist_remove(&sl, 0);
    assert(sl.count == 1);
    assert(strcmp(sl.items[0], "+79992223344") == 0);

    stringlist_remove(&sl, 99);
    assert(sl.count == 1);

    stringlist_free(&sl);
    assert(sl.items == NULL);
    assert(sl.count == 0);

    printf("[OK] test_stringlist_operations passed.\n");
}

void test_contact_init_and_free(void) {
    Contact c;
    contact_init(&c);

    assert(c.last_name[0] == '\0');
    assert(c.first_name[0] == '\0');
    assert(c.phones.count == 0);

    strcpy(c.last_name, "Иванов");
    strcpy(c.first_name, "Иван");
    stringlist_add(&c.phones, "12345");

    assert(strcmp(c.last_name, "Иванов") == 0);
    assert(c.phones.count == 1);

    contact_free(&c);
    assert(c.phones.count == 0);

    printf("[OK] test_contact_init_and_free passed.\n");
}

void test_contactlist_operations(void) {
    ContactList list;
    contactlist_init(&list);

    assert(list.count == 0);

    Contact* c1 = createContact();
    strcpy(c1->last_name, "Test1");
    strcpy(c1->first_name, "Test11");

    Contact* c2 = createContact();
    strcpy(c2->last_name, "Test2");
    strcpy(c2->first_name, "Test22");

    avl_insert_auto(&list, c1);
    avl_insert_auto(&list, c2);

    assert(list.count == 2);
    assert(list.root->data == c1);
    assert(get_contact_by_index(&list, 1) == c2);

    contactlist_remove(&list, 0);
    
    assert(list.count == 1);
    assert(list.root->data == c2);

    contactlist_remove(&list, 0);
    assert(list.count == 0);

    printf("[OK] test_contactlist_operations passed.\n");
}

int run_tests() {
    printf("Tests are running...\n");
    
    test_stringlist_operations();
    test_contact_init_and_free();
    test_contactlist_operations();

    printf("Tests done\n");
    return 0;
}
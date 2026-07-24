#include "pract6_1lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "phonebook.h"
#include "tests.h"

void start(void) {
    run_tests();
    ContactList list;
    contactlist_init(&list);
    while (true) {
        print_menu();
        int choice = read_int("Choose a menu item: ");

        switch (choice) {
            case 1:
                add_contact_flow(&list);
                break;
            case 2:
                list_contacts(&list);
                break;
            case 3:
                find_contacts_flow(&list);
                break;
            case 4:
                edit_contact_flow(&list);
                break;
            case 5:
                delete_contact_flow(&list);
                break;
            default:
                printf("Invalid menu item, please try again.\n");
        }
    }
}
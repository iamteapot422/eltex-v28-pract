#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "stringlist.h"

#define NAME_LEN   50
#define FIELD_LEN  100
#define LINE_LEN   256

typedef struct {
    char last_name[NAME_LEN];
    char first_name[NAME_LEN];
    char middle_name[NAME_LEN];
    char workplace[FIELD_LEN];
    char position[FIELD_LEN];
    StringList phones;
    StringList emails;
    StringList social_links;
    StringList messengers;
} Contact;

void contact_init(Contact* c) {
    c->last_name[0] = '\0';
    c->first_name[0] = '\0';
    c->middle_name[0] = '\0';
    c->workplace[0] = '\0';
    c->position[0] = '\0';
    stringlist_init(&c->phones);
    stringlist_init(&c->emails);
    stringlist_init(&c->social_links);
    stringlist_init(&c->messengers);
}

void contact_free(Contact* c) {
    stringlist_free(&c->phones);
    stringlist_free(&c->emails);
    stringlist_free(&c->social_links);
    stringlist_free(&c->messengers);
}

void contact_print(const Contact* c, int index) {
    printf("[%d] %s %s", index + 1, c->last_name, c->first_name);
    if (c->middle_name[0] != '\0') {
        printf(" %s", c->middle_name);
    }
    printf("\n");
    if (c->workplace[0] != '\0') {
        printf("  Workplace: %s\n", c->workplace);
    }
    if (c->position[0] != '\0') {
        printf("  Position: %s\n", c->position);
    }
    stringlist_print(&c->phones, "Phones");
    stringlist_print(&c->emails, "Emails");
    stringlist_print(&c->social_links, "Social links");
    stringlist_print(&c->messengers, "Messengers");
}

typedef struct {
    Contact* items;
    int count;
    int capacity;
} ContactList;

void contactlist_init(ContactList* list) {
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

Contact* contactlist_add(ContactList* list) {
    if (list->count == list->capacity) {
        int new_capacity = (list->capacity == 0) ? 4 : list->capacity * 2;
        list->items = realloc(list->items, new_capacity * sizeof(Contact));
        list->capacity = new_capacity;
    }
    contact_init(&list->items[list->count]);
    list->count++;
    return &list->items[list->count - 1];
}

void contactlist_remove(ContactList* list, int index) {
    if (index < 0 || index >= list->count) return;
    contact_free(&list->items[index]);
    for (int i = index; i < list->count - 1; i++) {
        list->items[i] = list->items[i + 1];
    }
    list->count--;
}


bool read_line(char* buffer, int size) {
    if (fgets(buffer, size, stdin) == NULL) {
        buffer[0] = '\0';
        return false;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    return true;
}

void read_required_line(const char* prompt, char* buffer, int size) {
    while (true) {
        printf("%s", prompt);
        read_line(buffer, size);
        if (buffer[0] != '\0') {
            return;
        }
        printf("This field is required, please enter a value.\n");
    }
}

int read_int(const char* prompt) {
    char buf[LINE_LEN];
    int value;
    while (true) {
        printf("%s", prompt);
        read_line(buf, sizeof(buf));
        if (sscanf(buf, "%d", &value) == 1) {
            return value;
        }
        printf("Please enter a valid number.\n");
    }
}

void fill_stringlist_from_input(StringList* sl, const char* fieldname) {
    char buf[LINE_LEN];
    printf("Enter %s:\n", fieldname);
    while (true) {
        printf("  > ");
        read_line(buf, sizeof(buf));
        if (buf[0] == '\0') break;
        stringlist_add(sl, buf);
    }
}

void edit_stringlist_menu(StringList* sl, const char* fieldname) {
    while (true) {
        printf("\nEditing field \"%s\"\n", fieldname);
        stringlist_print(sl, fieldname);
        printf("1. Add value\n");
        printf("2. Remove value\n");
        printf("0. Back\n");
        int choice = read_int("Choose an action: ");
        char buf[LINE_LEN];
        switch (choice) {
            case 1:
                printf("Enter new value: ");
                read_line(buf, sizeof(buf));
                if (buf[0] != '\0') {
                    stringlist_add(sl, buf);
                    printf("Value added.\n");
                }
                break;
            case 2:
                if (sl->count == 0) {
                    printf("The list is empty, nothing to remove.\n");
                    break;
                }
                int idx = read_int("Enter the number of the value to remove: ") - 1;
                if (idx < 0 || idx >= sl->count) {
                    printf("Invalid number.\n");
                } else {
                    stringlist_remove(sl, idx);
                    printf("Value removed.\n");
                }
                break;
            case 0:
                return;
            default:
                printf("Invalid choice.\n");
        }
    }
}

void add_contact_flow(ContactList* list) {
    Contact* c = contactlist_add(list);
    if (c == NULL) return;

    printf("\nAdding a new contact\n");
    read_required_line("Last name: ", c->last_name, NAME_LEN);
    read_required_line("First name: ", c->first_name, NAME_LEN);

    printf("Middle name (optional, press Enter to skip): ");
    read_line(c->middle_name, NAME_LEN);

    printf("Workplace (optional): ");
    read_line(c->workplace, FIELD_LEN);

    printf("Position (optional): ");
    read_line(c->position, FIELD_LEN);

    fill_stringlist_from_input(&c->phones, "phone numbers");
    fill_stringlist_from_input(&c->emails, "email addresses");
    fill_stringlist_from_input(&c->social_links, "social media links");
    fill_stringlist_from_input(&c->messengers, "messenger profiles");

    printf("Contact added successfully.\n");
}

void list_contacts(const ContactList* list) {
    printf("\nContact list (%d)\n", list->count);
    for (int i = 0; i < list->count; i++) {
        contact_print(&list->items[i], i);
    }
}

void find_contacts_flow(const ContactList* list) {
    char query[NAME_LEN];
    printf("Enter last name or first name to search: ");
    read_line(query, sizeof(query));

    bool found = false;
    for (int i = 0; i < list->count; i++) {
        if (strstr(list->items[i].last_name, query) != NULL ||
            strstr(list->items[i].first_name, query) != NULL) {
            contact_print(&list->items[i], i);
            found = true;
        }
    }
    if (!found) {
        printf("No matches found.\n");
    }
}

int choose_contact_index(const ContactList* list) {
    list_contacts(list);
    int idx = read_int("Enter contact number (0 - cancel): ");
    if (idx == 0) return -1;
    idx -= 1;
    if (idx < 0 || idx >= list->count) {
        printf("Number is out of bounds.\n");
        return -1;
    }
    return idx;
}

void edit_contact_flow(ContactList* list) {
    int idx = choose_contact_index(list);
    if (idx == -1) return;
    Contact* c = &list->items[idx];

    while (true) {
        contact_print(c, idx);
        printf("1. Change last name\n");
        printf("2. Change first name\n");
        printf("3. Change middle name\n");
        printf("4. Change workplace\n");
        printf("5. Change position\n");
        printf("6. Edit phones\n");
        printf("7. Edit emails\n");
        printf("8. Edit social links\n");
        printf("9. Edit messengers\n");
        printf("0. Done (return to main menu)\n");
        int choice = read_int("Choose an action: ");

        switch (choice) {
            case 1:
                read_required_line("New last name: ", c->last_name, NAME_LEN);
                break;
            case 2:
                read_required_line("New first name: ", c->first_name, NAME_LEN);
                break;
            case 3:
                printf("New middle name (press Enter to leave empty): ");
                read_line(c->middle_name, NAME_LEN);
                break;
            case 4:
                printf("New workplace: ");
                read_line(c->workplace, FIELD_LEN);
                break;
            case 5:
                printf("New position: ");
                read_line(c->position, FIELD_LEN);
                break;
            case 6:
                edit_stringlist_menu(&c->phones, "Phones");
                break;
            case 7:
                edit_stringlist_menu(&c->emails, "Emails");
                break;
            case 8:
                edit_stringlist_menu(&c->social_links, "Social links");
                break;
            case 9:
                edit_stringlist_menu(&c->messengers, "Messengers");
                break;
            case 0:
                printf("Changes saved.\n");
                return;
            default:
                printf("Invalid choice.\n");
        }
    }
}

void delete_contact_flow(ContactList* list) {
    int idx = choose_contact_index(list);
    if (idx == -1) return;
    contactlist_remove(list, idx);
    printf("Contact deleted.\n");
}


void print_menu(void) {
    printf("1. Add contact\n");
    printf("2. Show all contacts\n");
    printf("3. Find contact (by last/first name)\n");
    printf("4. Edit contact\n");
    printf("5. Delete contact\n");
    printf("0. Exit\n");
}


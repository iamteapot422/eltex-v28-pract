//
// Created by iamteapot on 22.07.2026.
//

#ifndef PRACT2_1_PHONEBOOK_H
#define PRACT2_1_PHONEBOOK_H

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

void contact_init(Contact* c);
void contact_free(Contact* c);
void contact_print(const Contact* c, int index);

typedef struct ContactEntry {
    struct ContactEntry* prev;
    struct ContactEntry* next;
    Contact* data;
} ContactEntry;

typedef struct {
    ContactEntry* head;
    ContactEntry* tail;
    int count;
} ContactList;

void contactlist_init(ContactList* list);
ContactEntry* createContactEntry();
void contactlist_remove(ContactList* list, int index);

void moveToRightPlace(ContactList* list, ContactEntry* entry);
void insertEntry(ContactList* list, ContactEntry* entryToInsert);
ContactEntry* getContactEntryByIndex(ContactList* list, int index);
bool read_line(char* buffer, int size);
void read_required_line(const char* prompt, char* buffer, int size);
int read_int(const char* prompt);
void fill_stringlist_from_input(StringList* sl, const char* fieldname);
void edit_stringlist_menu(StringList* sl, const char* fieldname);

void add_contact_flow(ContactList* list);
void list_contacts(const ContactList* list);
void find_contacts_flow(const ContactList* list);
int choose_contact_index(const ContactList* list);
void edit_contact_flow(ContactList* list);
void delete_contact_flow(ContactList* list);

void print_menu(void);


#endif //PRACT2_1_PHONEBOOK_H

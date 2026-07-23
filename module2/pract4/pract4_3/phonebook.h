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

typedef struct AVLNode {
    Contact* data;
    struct AVLNode* left;
    struct AVLNode* right;
    int height;
} AVLNode;

typedef struct {
    AVLNode* root;
    int count;
} ContactList;

void contactlist_init(ContactList* list);
void contactlist_free(ContactList* list);
Contact* createContact(void);
void contactlist_remove(ContactList* list, int index);
Contact* get_contact_by_index(ContactList* list, int index);
void avl_insert_auto(ContactList* list, Contact* c);

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

#endif
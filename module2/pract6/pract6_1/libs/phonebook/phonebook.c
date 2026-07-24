#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../stringlist.h"
#include "../phonebook.h"

#define NAME_LEN   50
#define FIELD_LEN  100
#define LINE_LEN   256

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

static int contact_compare(const Contact* a, const Contact* b) {
    int res = strcmp(a->first_name, b->first_name);
    if (res != 0) return res;
    res = strcmp(a->last_name, b->last_name);
    if (res != 0) return res;
    res = strcmp(a->middle_name, b->middle_name);
    if (res != 0) return res;
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

static int get_height(AVLNode* n) {
    return n ? n->height : 0;
}

static int max_val(int a, int b) {
    return (a > b) ? a : b;
}

static void update_height(AVLNode* n) {
    if (n) {
        n->height = 1 + max_val(get_height(n->left), get_height(n->right));
    }
}

static int get_balance(AVLNode* n) {
    return n ? get_height(n->left) - get_height(n->right) : 0;
}

static AVLNode* rotate_right(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    update_height(y);
    update_height(x);
    return x;
}

static AVLNode* rotate_left(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    update_height(x);
    update_height(y);
    return y;
}

static AVLNode* avl_insert(AVLNode* node, Contact* data) {
    if (node == NULL) {
        AVLNode* new_node = malloc(sizeof(AVLNode));
        new_node->data = data;
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->height = 1;
        return new_node;
    }
    int cmp = contact_compare(data, node->data);
    if (cmp < 0) {
        node->left = avl_insert(node->left, data);
    } else if (cmp > 0) {
        node->right = avl_insert(node->right, data);
    } else {
        return node;
    }

    update_height(node);
    int balance = get_balance(node);

    if (balance > 1 && contact_compare(data, node->left->data) < 0) {
        return rotate_right(node);
    }
    if (balance < -1 && contact_compare(data, node->right->data) > 0) {
        return rotate_left(node);
    }
    if (balance > 1 && contact_compare(data, node->left->data) > 0) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    if (balance < -1 && contact_compare(data, node->right->data) < 0) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }

    return node;
}

static AVLNode* min_value_node(AVLNode* node) {
    AVLNode* current = node;
    while (current->left != NULL) {
        current = current->left;
    }
    return current;
}

static AVLNode* avl_delete(AVLNode* root, const Contact* data) {
    if (root == NULL) return root;

    int cmp = contact_compare(data, root->data);
    if (cmp < 0) {
        root->left = avl_delete(root->left, data);
    } else if (cmp > 0) {
        root->right = avl_delete(root->right, data);
    } else {
        if ((root->left == NULL) || (root->right == NULL)) {
            AVLNode* temp = root->left ? root->left : root->right;
            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else {
                *root = *temp;
            }
            free(temp);
        } else {
            AVLNode* temp = min_value_node(root->right);
            root->data = temp->data;
            root->right = avl_delete(root->right, temp->data);
        }
    }

    if (root == NULL) return root;

    update_height(root);
    int balance = get_balance(root);

    if (balance > 1 && get_balance(root->left) >= 0) {
        return rotate_right(root);
    }
    if (balance > 1 && get_balance(root->left) < 0) {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }
    if (balance < -1 && get_balance(root->right) <= 0) {
        return rotate_left(root);
    }
    if (balance < -1 && get_balance(root->right) > 0) {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }

    return root;
}

void avl_insert_auto(ContactList* list, Contact* c) {
    list->root = avl_insert(list->root, c);
    list->count++;
}

void contactlist_init(ContactList* list) {
    list->root = NULL;
    list->count = 0;
}

static void free_tree_helper(AVLNode* root) {
    if (root == NULL) return;
    free_tree_helper(root->left);
    free_tree_helper(root->right);
    contact_free(root->data);
    free(root->data);
    free(root);
}

void contactlist_free(ContactList* list) {
    free_tree_helper(list->root);
    list->root = NULL;
    list->count = 0;
}

Contact* createContact(void) {
    Contact* newContact = malloc(sizeof(Contact));
    contact_init(newContact);
    return newContact;
}

static void get_by_index_helper(AVLNode* root, int target, int* current, Contact** result) {
    if (root == NULL || *result != NULL) return;
    get_by_index_helper(root->left, target, current, result);
    if (*result != NULL) return;
    if (*current == target) {
        *result = root->data;
        return;
    }
    (*current)++;
    get_by_index_helper(root->right, target, current, result);
}

Contact* get_contact_by_index(ContactList* list, int index) {
    if (index < 0 || index >= list->count) return NULL;
    int current = 0;
    Contact* result = NULL;
    get_by_index_helper(list->root, index, &current, &result);
    return result;
}

void contactlist_remove(ContactList* list, int index) {
    Contact* c = get_contact_by_index(list, index);
    if (c == NULL) return;
    list->root = avl_delete(list->root, c);
    contact_free(c);
    free(c);
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
    Contact* c = createContact();
    if (c == NULL) return;

    printf("\nAdding a new contact\n");
    //read_required_line("Last name: ", c->last_name, NAME_LEN);
    read_required_line("First name: ", c->first_name, NAME_LEN);

    /*printf("Middle name (optional, press Enter to skip): ");
    read_line(c->middle_name, NAME_LEN);

    printf("Workplace (optional): ");
    read_line(c->workplace, FIELD_LEN);

    printf("Position (optional): ");
    read_line(c->position, FIELD_LEN);

    fill_stringlist_from_input(&c->phones, "phone numbers");
    fill_stringlist_from_input(&c->emails, "email addresses");
    fill_stringlist_from_input(&c->social_links, "social media links");
    fill_stringlist_from_input(&c->messengers, "messenger profiles");*/

    avl_insert_auto(list, c);
    printf("Contact added successfully.\n");
}

static void list_contacts_helper(AVLNode* root, int* index) {
    if (root == NULL) return;
    list_contacts_helper(root->left, index);
    contact_print(root->data, *index);
    (*index)++;
    list_contacts_helper(root->right, index);
}

void list_contacts(const ContactList* list) {
    printf("\nContact list (%d)\n", list->count);
    int index = 0;
    list_contacts_helper(list->root, &index);
}

static void find_contacts_helper(AVLNode* root, const char* query, bool* found, int* index) {
    if (root == NULL) return;
    find_contacts_helper(root->left, query, found, index);
    if (strstr(root->data->last_name, query) != NULL ||
        strstr(root->data->first_name, query) != NULL) {
        contact_print(root->data, *index);
        *found = true;
    }
    (*index)++;
    find_contacts_helper(root->right, query, found, index);
}

void find_contacts_flow(const ContactList* list) {
    char query[NAME_LEN];
    printf("Enter last name or first name to search: ");
    read_line(query, sizeof(query));

    bool found = false;
    int index = 0;
    find_contacts_helper(list->root, query, &found, &index);
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
    Contact* c = get_contact_by_index(list, idx);
    if (c == NULL) return;

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

        if (choice == 0) {
            printf("Changes saved.\n");
            return;
        }

        if (choice >= 1 && choice <= 3) {
            list->root = avl_delete(list->root, c);
            if (choice == 1) {
                read_required_line("New last name: ", c->last_name, NAME_LEN);
            } else if (choice == 2) {
                read_required_line("New first name: ", c->first_name, NAME_LEN);
            } else if (choice == 3) {
                printf("New middle name (press Enter to leave empty): ");
                read_line(c->middle_name, NAME_LEN);
            }
            list->root = avl_insert(list->root, c);
        } else {
            switch (choice) {
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
                default:
                    printf("Invalid choice.\n");
            }
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
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>

typedef struct {
    char key;
    double (*operation)(double, double);
} operation_item;

typedef struct {
    operation_item* operations;
    int count;
    int capacity;
} operation_array;

operation_item* linear_search(const operation_array* const array, char key) {
    for (int i = 0; i < array->count; i++) {
        if (array->operations[i].key == key) {
            return &array->operations[i];
        }
    }
    return NULL;
}
void add_operation(operation_array* array, char key, double (*operation)(double, double)) {
    if (array->count >= array->capacity) {
        int new_capacity;
        if (array->capacity == 0) {
            new_capacity = 1;
        }
        else {
            new_capacity = array->capacity * 2;
        }
        operation_item* tmp = realloc(array->operations, new_capacity * sizeof(operation_item));
        array->operations = tmp;
        array->capacity = new_capacity;
    }
    array->operations[array->count].key = key;
    array->operations[array->count].operation = operation;
    array->count++;
}
int main(void) {
    char* libdir = "./libs";
    
    while (true)
    {
        DIR* dir = opendir(libdir);
        struct dirent* entry;
        void *handle;
        void **handles = malloc(sizeof(void**));
        int handle_count = 0;
        double (*operation)(double, double);
        operation_array operations = {0};
        char *error;
        printf("Available operations: ");
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            
            char filepath[1024];
            snprintf(filepath, sizeof(filepath), "%s/%s/lib%s.so", libdir, entry->d_name, entry->d_name);
            handle = dlopen(filepath, RTLD_LAZY);
            if (!handle) {
                continue;
            }
            handle_count++;
            handles = (void**)realloc(handles, handle_count * sizeof(void**));
            handles[handle_count - 1] = handle;
            
            operation = dlsym(handle, entry->d_name);
            char* operationsymb = dlsym(handle, "operation");
            
            printf("%s", operationsymb);
            add_operation(&operations, *operationsymb, operation);
        }
        printf("\n");
        double a, b = 0;
        char op;
        scanf("%lf %c %lf", &a, &op, &b);
        
        operation_item* item = linear_search(&operations, op);
        if (item)
        {
            printf("%g\n", item->operation(a, b));
        }
        else
        {
            printf("Operation not found\n");
        }
        for (int i = 0; i < handle_count; i++)
        {
          dlclose(handles[i]);
        }
        
        free(operations.operations);
        free(handles);
    }
}

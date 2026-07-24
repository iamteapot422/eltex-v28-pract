#include <stdio.h>
#include <stdlib.h>

#include "addition.h"
#include "multiplication.h"
#include "substraction.h"
#include "division.h"

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
    return &array->operations[0];
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
    double a, b = 0;
    char op = '+';
    operation_array operations = {0};
    add_operation(&operations, '+', plus);
    add_operation(&operations, '-', minus);
    add_operation(&operations, '*', multiply);
    add_operation(&operations, '/', divide);
    add_operation(&operations, '<', minus);
    scanf("%lf %c %lf", &a, &op, &b);
    printf("%g", linear_search(&operations, op)->operation(a, b));
    return 0;
}

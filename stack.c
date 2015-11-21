#include "stack.h"

const size_t STACK_START_CAP = 8;

stack_t* _stack_init(bool ptable_insert) {
    stack_t* stack = _ifj15_malloc(STACK, sizeof(stack_t), ptable_insert);
    stack->size = STACK_START_CAP;
    stack->top = 0;
    stack->data = _ifj15_malloc(ARRAY, sizeof(char)*STACK_START_CAP, false);
    return stack;
}

void _stack_free(stack_t* stack) {
    free(stack->data);
    free(stack);
}

void stack_resize(stack_t* stack, size_t new_size) {
    stack->data = _ifj15_realloc(stack->data, sizeof(char)*new_size, false);
    stack->size = new_size;
}

void _stack_push_char(stack_t* stack, char c) {
    if (stack->top == stack->size)
        stack_resize(stack, stack->size*2);

    stack->data[(stack->top)++] = c;
}

int _stack_top_char(stack_t* stack, bool remove_top) {
    if (stack->top == 0) {
        warning("stack_pop/stack_get: Tried to pop/get item from an empty stack");
        return -1;
    }

    char top_char = stack->data[stack->top - 1];
    if (remove_top) --(stack->top);
    return top_char;
}

int stack_find(stack_t* stack, stack_compare cmp) {
    int found = -1;
    for (int i = stack->top - 1; i >= 0; --i) {
        if (cmp(stack->data[i])) {
           found = stack->data[i];
           break;
        }
    }
    return found;
}

// for pos: pos < stack->top
void stack_insert(stack_t* stack, char c, size_t pos) {
    _stack_push_char(stack, stack->data[stack->top - 1]);
    for (int i = stack->top - 2; i > pos; --i)
        stack->data[i] = stack->data[i - 1];
    stack->data[pos] = c;
}

void stack_insert_after(stack_t* stack, char c, stack_compare cmp) {

    for (int i = stack->top - 1; i >= 0; --i) {
        if (cmp(stack->data[i])) {
           (i == stack->top - 1) ? _stack_push_char(stack, c) : stack_insert(stack, c, i + 1);
           break;
        }
    }
}

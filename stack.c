#include "stack.h"

const size_t STACK_START_CAP = 8;

void* _stack_init(type_t stack_type, bool ptable_insert) {
    if (stack_type == CHAR) {
        stack_char_t* stack = _ifj15_malloc(STACK, sizeof(stack_char_t), ptable_insert);
        stack->data = _ifj15_malloc(ARRAY, sizeof(char)*STACK_START_CAP, false);
        stack->size = STACK_START_CAP;
        stack->top = 0;
        return stack;
    } else if (stack_type == TOKEN) {
        stack_token_t* stack = _ifj15_malloc(STACK, sizeof(stack_token_t), ptable_insert);
        stack->data = _ifj15_malloc(ARRAY, sizeof(token_t)*STACK_START_CAP, false);
        stack->size = STACK_START_CAP;
        stack->top = 0;
        return stack;
    } else {
        error("Unknown stack type", ERROR_INTERNAL);
    }
    return NULL;
}

void _stack_free(void* stack) {
    free(((stack_char_t*)stack)->data);
    free(stack);
}

void stack_resize_char(stack_char_t* stack, size_t new_size) {
    stack->data = _ifj15_realloc(stack->data, sizeof(char)*new_size, false);
    stack->size = new_size;
}
void stack_resize_token(stack_token_t* stack, size_t new_size) {
    stack->data = _ifj15_realloc(stack->data, sizeof(token_t)*new_size, false);
    stack->size = new_size;
}

void _stack_push_char(stack_char_t* stack, char c) {
    if (stack->top == stack->size)
        stack_resize_char(stack, stack->size*2);

    stack->data[(stack->top)++] = c;
}
void _stack_push_token(stack_token_t* stack, token_t token) {
    if (stack->top == stack->size)
        stack_resize_token(stack, stack->size*2);

    stack->data[(stack->top)++] = token;
}

int _stack_top_char(stack_char_t* stack, bool remove_top) {
    if (stack->top == 0) {
        warning("stack_pop/stack_get: Tried to pop/get item from an empty stack");
        return -1;
    }

    char top_char = stack->data[stack->top - 1];
    if (remove_top) --(stack->top);
    return top_char;
}
token_t _stack_top_token(stack_token_t* stack, bool remove_top) {
    if (stack->top == 0) {
        warning("stack_pop/stack_get: Tried to pop/get item from an empty stack");
        token_t ret = {.type = TT_NONE};
        return ret;
    }

    token_t top_token = stack->data[stack->top - 1];
    if (remove_top) --(stack->top);
    return top_token;
}

int stack_find(stack_char_t* stack, stack_compare cmp) {
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
void stack_insert(stack_char_t* stack, char c, size_t pos) {
    _stack_push_char(stack, stack->data[stack->top - 1]);
    for (int i = stack->top - 2; i > pos; --i)
        stack->data[i] = stack->data[i - 1];
    stack->data[pos] = c;
}

void stack_insert_after(stack_char_t* stack, char c, stack_compare cmp) {

    for (int i = stack->top - 1; i >= 0; --i) {
        if (cmp(stack->data[i])) {
           (i == stack->top - 1) ? _stack_push_char(stack, c) : stack_insert(stack, c, i + 1);
           break;
        }
    }
}

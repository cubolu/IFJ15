#ifndef STACK_H
#define STACK_H

#include "common.h"
#include "error.h"
#include "memory.h"

struct _stack_t {
    size_t size;
    size_t top;
    char* data;
};

#define stack_init() _stack_init(true)
#define stack_push(stack, item) _stack_push_char(stack, item)
#define stack_pop(stack) _stack_top_char(stack, true)
#define stack_get(stack) _stack_top_char(stack, false)

stack_t* _stack_init(bool ptable_insert);
void _stack_free(stack_t* stack);

void _stack_push_char(stack_t* stack, char c);
int _stack_top_char(stack_t* stack, bool remove_top);

/* search for item in stack from top to bottom, item must fulfill
 * user defined cmp function */
int stack_find(stack_t* stack, stack_compare cmp);
/* insert item c directly after first item, that fulfill cmp function */
void stack_insert_after(stack_t* stack, char c, stack_compare cmp);

#endif

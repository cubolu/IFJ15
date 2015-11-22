#ifndef STACK_H
#define STACK_H

#include "common.h"
#include "error.h"
#include "memory.h"

typedef enum {SI_CHAR, SI_HTABLE, SI_TOKEN} stack_item_t;

struct _stack_char_t {
    size_t size;
    size_t top;
    char* data;
};

struct _stack_htable_t {
    size_t size;
    size_t top;
    htable_t** data;
};

struct _stack_token_t {
    size_t size;
    size_t top;
    token_t* data;
};

#define stack_init(type) _stack_init(type, true)

#define stack_push(stack, item) _Generic((stack), \
      stack_char_t* : _stack_push_char, \
      stack_htable_t* : _stack_push_htable, \
      stack_token_t* : _stack_push_token)(stack, item)

#define stack_pop(stack) _Generic((stack), \
      stack_char_t* : _stack_top_char, \
      stack_htable_t* : _stack_top_htable, \
      stack_token_t* : _stack_top_token)(stack, true)

#define stack_get(stack) _Generic((stack), \
      stack_char_t* : _stack_top_char, \
      stack_htable_t* : _stack_top_htable, \
      stack_token_t* : _stack_top_token)(stack, false)

#define stack_find(stack, cmp) _Generic((stack), \
      stack_char_t* : _stack_find(stack, cmp), \
      default : error("Unsupported stack operation", ERROR_INTERNAL))

#define stack_insert_after(stack, c, cmp) _Generic((stack), \
      stack_char_t* : _stack_insert_after(stack, c, cmp), \
      default : error("Unsupported stack operation", ERROR_INTERNAL))

void* _stack_init(stack_item_t si, bool ptable_insert);
void _stack_free(void* stack);

void _stack_push_char(stack_char_t* stack, char c);
void _stack_push_htable(stack_htable_t* stack, htable_t* h);
void _stack_push_token(stack_token_t* stack, token_t t);
int _stack_top_char(stack_char_t* stack, bool remove_top);
htable_t* _stack_top_htable(stack_htable_t* stack, bool remove_top);
token_t _stack_top_token(stack_token_t* stack, bool remove_top);

/* search for item in stack from top to bottom, item must fulfill
 * user defined cmp function */
int _stack_find(stack_char_t* stack, stack_compare cmp);
/* insert item c directly after first item, that fulfill cmp function */
void _stack_insert_after(stack_char_t* stack, char c, stack_compare cmp);

#endif

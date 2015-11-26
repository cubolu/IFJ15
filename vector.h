#ifndef VECTOR_H
#define VECTOR_H

#include "common.h"

typedef enum {VI_CHAR, VI_INT, VI_EXPR, VI_HTABLE, VI_TOKEN} vector_item_t;

struct _vector_char_t {
    size_t capacity;
    size_t size;
    char* array;
};

struct _vector_int_t {
    size_t capacity;
    size_t size;
    int* array;
};

struct _vector_expr_t {
    size_t capacity;
    size_t size;
    expression_t* array;
};

struct _vector_htable_t {
    size_t capacity;
    size_t size;
    htable_t** array;
};

struct _vector_token_t {
    size_t capacity;
    size_t size;
    token_t* array;
};

#include "memory.h"
#include "error.h"
#include "symbol.h"

#define vector_init1(type) _vector_init(type, 8, true)
#define vector_init2(type, start_cap) _vector_init(type, start_cap, true)

#define vector_init(...) VFUNC(vector_init, __VA_ARGS__)

#define vector_push(vct, item) _Generic((vct),          \
    vector_char_t*      : _vector_push_char,            \
    vector_int_t*       : _vector_push_int,             \
    vector_expr_t*      : _vector_push_expr,            \
    vector_htable_t*    : _vector_push_htable,          \
    vector_token_t*     : _vector_push_token)(vct, item)

#define vector_pop(vct) _Generic((vct),                 \
    vector_char_t*      : _vector_top_char,             \
    vector_int_t*       : _vector_top_int,              \
    vector_expr_t*      : _vector_top_expr,             \
    vector_htable_t*    : _vector_top_htable,           \
    vector_token_t*     : _vector_top_token)(vct, true)

#define vector_top(vct) _Generic((vct),                 \
    vector_char_t*      : _vector_top_char,             \
    vector_int_t*       : _vector_top_int,              \
    vector_expr_t*      : _vector_top_expr,             \
    vector_htable_t*    : _vector_top_htable,           \
    vector_token_t*     : _vector_top_token)(vct, false)

#define vector_at(vct, pos) _Generic((vct),             \
    vector_char_t*      : _vector_at_char,              \
    vector_int_t*       : _vector_at_int,               \
    vector_expr_t*      : _vector_at_expr,              \
    vector_htable_t*    : _vector_at_htable,            \
    vector_token_t*     : _vector_at_token)(vct, pos)

#define vector_find(vct, cmp) _Generic((vct),           \
    vector_char_t*      : _vector_find_char,            \
    vector_htable_t*    : _vector_find_symbol)(vct, cmp)

#define vector_insert_after(vct, c, cmp) _Generic((vct), \
    vector_char_t*      : _vector_insert_after(vct, c, cmp))

void* _vector_init(vector_item_t si, size_t start_cap, bool ptable_insert);
void _vector_free(void* vct);

void _vector_push_char(vector_char_t* vct, char c);
void _vector_push_int(vector_int_t* vct, int i);
void _vector_push_expr(vector_expr_t* vct, expression_t t);
void _vector_push_htable(vector_htable_t* vct, htable_t* h);
void _vector_push_token(vector_token_t* vct, token_t t);

int _vector_top_char(vector_char_t* vct, bool remove_top);
int _vector_top_int(vector_int_t* vct, bool remove_top);
expression_t _vector_top_expr(vector_expr_t* vct, bool remove_top);
htable_t* _vector_top_htable(vector_htable_t* vct, bool remove_top);
token_t _vector_top_token(vector_token_t* vct, bool remove_top);

int _vector_at_char(vector_char_t* vct, size_t pos);
int _vector_at_int(vector_int_t* vct, size_t pos);
expression_t _vector_at_expr(vector_expr_t* vct, size_t pos);
htable_t* _vector_at_htable(vector_htable_t* vct, size_t pos);
token_t _vector_at_token(vector_token_t* vct, size_t pos);

/* search for item in vector from top to bottom, item must fulfill
 * user defined cmp function */
int _vector_find_char(vector_char_t* vct, vector_compare cmp);

symbol_t* _vector_find_symbol(vector_htable_t* vct, str_t* name);

/* insert item c directly after first item, that fulfill cmp function */
void _vector_insert_after(vector_char_t* vct, char c, vector_compare cmp);

#endif

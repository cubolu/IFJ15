#ifndef ULIST_H
#define ULIST_H

#include "common.h"

struct _ulist_t {
    unode_t* front;
    unode_t* back;
};

struct _ulist_str_t {
    unode_str_t* front;
    unode_str_t* back;
};

#include "memory.h"

struct _unode_t {
    void* key;
    ptr_t item;
    unode_t* next;
    unode_t* prev;
};

#include "symbol.h"

struct _unode_str_t {
    str_t* key;
    symbol_t item;
    unode_str_t* next;
    unode_str_t* prev;
};

#include "error.h"

#define ulist_set(ulist, key, item) _Generic((key), \
    str_t* : _ulist_str_set, \
    void*: _ulist_set)(ulist,key,item)

#define ulist_pop(ulist, key) _Generic((key), \
    str_t* : _ulist_str_pop, \
    void*: _ulist_pop)(ulist, key)

#define ulist_get(ulist, key) _Generic((key), \
    str_t* : _ulist_str_get, \
    default: _ulist_get)(ulist, key)

#define ulist_str_init() _ulist_str_init(true);
#define ulist_init() _ulist_init(true);

ulist_t* _ulist_init(bool ptable_insert);
ulist_str_t* _ulist_str_init(bool ptable_insert);
void _ulist_free(ulist_t* ulist);
void _ulist_str_free(ulist_str_t* ulist);

void _ulist_set(ulist_t* ulist, void* key, ptr_t item);
void _ulist_str_set(ulist_str_t* ulist, str_t* key, symbol_t item);
ptr_t _ulist_get(ulist_t* ulist, void* key);
ptr_t _ulist_pop(ulist_t* ulist, void* key);
symbol_t* _ulist_str_get(ulist_str_t* ulist, str_t* key);
symbol_t _ulist_str_pop(ulist_str_t* ulist, str_t* key);

#endif

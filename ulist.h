#ifndef ULIST_H
#define ULIST_H

#include "common.h"
#include "error.h"
#include "memory.h"

typedef enum {POINTER, CHARARRAY} key_t;

struct _unode_t {
    void* key;
    void* item;
    unode_t* next;
    unode_t* prev;
};

struct _ulist_t {
    unode_t* front;
    unode_t* back;
};

#define ulist_set(ulist, key, item) _Generic((key), \
    char* : _ulist_set(ulist, key, item, CHARARRAY), \
    default: _ulist_set(ulist, key, item, POINTER))

#define ulist_pop(ulist, key) _Generic((key), \
    char* : _ulist_get(ulist, key, CHARARRAY, true), \
    default: _ulist_get(ulist, key, POINTER, true))

#define ulist_get(ulist, key) _Generic((key), \
    char* : _ulist_get(ulist, key, CHARARRAY, false), \
    default: _ulist_get(ulist, key, POINTER, false))

#define ulist_init() _ulist_init(true)

ulist_t* _ulist_init(bool ptable_insert);
void _ulist_free(ulist_t* ulist);

void _ulist_set(ulist_t* ulist, void* key, void* item, key_t key_type);
void* _ulist_get(ulist_t* ulist, void* key, key_t key_type, bool remove_item);

#endif

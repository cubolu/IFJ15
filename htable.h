#ifndef HTABLE_H
#define HTABLE_H

#include "common.h"
#include "ulist.h"
#include "memory.h"
#include "error.h"

struct _htable_t {
    ulist_t** array;
    //For indexing into HTABLE_SIZES
    size_t cap_index;
    size_t size;
};

htable_t* htable_init();
void _htable_free(htable_t* htable);

void htable_set(htable_t* htable, char* key, void* item);
void* htable_get(htable_t* htable, char* key);

void htable_remove(htable_t* htable, char* key);
void* htable_pop(htable_t* htable, char* key);

#endif

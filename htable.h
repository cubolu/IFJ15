#ifndef HTABLE_H
#define HTABLE_H

#include "common.h"
#include "ulist.h"
#include "memory.h"
#include "error.h"
#include "str.h"

struct _htable_t {
    ulist_str_t** array;
    //For indexing into HTABLE_SIZES
    size_t cap_index;
    size_t size;
};

htable_t* htable_init();
void _htable_free(htable_t* htable);

void htable_set(htable_t* htable, str_t* key, symbol_t item);
symbol_t* htable_get(htable_t* htable, str_t* key);

void htable_remove(htable_t* htable, str_t* key);
symbol_t htable_pop(htable_t* htable, str_t* key);

#endif

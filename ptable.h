#ifndef PTABLE_H
#define PTABLE_H

#include "common.h"
#include "memory.h"
#include "ulist.h"

struct _ptable_t {
    ulist_t** array;
    // size of ptable is 2^capacity_pow
    size_t capacity_pow;
    size_t size;
};

#define ptable_init() _ptable_init(true)

ptable_t* _ptable_init(bool ptable_insert);
void _ptable_free(ptable_t* ptable);

void ptable_insert(ptable_t* ptable, void* ptr, ptr_t type);
ptr_t ptable_pop(ptable_t* ptable, void* ptr);

#endif

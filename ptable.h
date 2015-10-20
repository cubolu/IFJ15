#ifndef PTABLE_H
#define PTABLE_H

#include "common.h"
#include "memory.h"
#include "list.h"

struct _ptable_t {
	list_t** array;
	// size of ptable is 2^capacity_pow
	size_t capacity_pow;
	size_t size;
};

ptable_t* ptable_init();
void ptable_free(ptable_t* ptable);

void ptable_insert(ptable_t* ptable, void* ptr, ptr_t type);
ptr_t ptable_pop(ptable_t* ptable, void* ptr);

#endif

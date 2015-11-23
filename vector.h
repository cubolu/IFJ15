#ifndef LIST_H
#define LIST_H

#include "common.h"

struct _vector_t {
    void** array;
    size_t size;
    size_t capacity;
    int start;
};

#include "error.h"
#include "memory.h"

vector_t* vector_init();
vector_t* vector_init_size(size_t init_size);
void _vector_free(vector_t* vector);

void vector_push_back(vector_t* vector, void* item);
void vector_push_front(vector_t* vector, void* item);
void* vector_pop_back(vector_t* vector);
void* vector_pop_front(vector_t* vector);

void* vector_at(vector_t* vector, size_t pos);

#endif

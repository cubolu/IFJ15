#include "vector.h"

typedef enum {UP, DOWN} resize_t;
const size_t VECTOR_START_CAP = 16;

void vector_resize(vector_t* vector, resize_t resize);
size_t real_pos(size_t capacity, int start, size_t pos);

vector_t* vector_init() {
    vector_t* vector = _ifj15_malloc(VECTOR, sizeof(vector_t), true);
    vector->array = _ifj15_malloc(ARRAY, sizeof(void*)*VECTOR_START_CAP, false);
    vector->size = 0;
    vector->capacity = VECTOR_START_CAP;
    vector->start = 0;
    return vector;
}

void _vector_free(vector_t* vector) {
    free(vector->array);
    free(vector);
}

void vector_push_front(vector_t* vector, void* item) {
    if (vector->size == vector->capacity)
        vector_resize(vector, UP);
    size_t pos = real_pos(vector->capacity, --(vector->start), 0);
    vector->array[pos] = item;
    ++(vector->size);
}

void vector_push_back(vector_t* vector, void* item) {
    if (vector->size == vector->capacity) {
        vector_resize(vector, UP);
    }
    size_t pos = real_pos(vector->capacity, vector->start, vector->size);
    vector->array[pos] = item;
    ++(vector->size);
}

void* vector_pop_front(vector_t* vector) {
    if (vector->size == 0) {
        warning("vector_pop_front: Tried to pop an empty vector");
        return 0;
    }
    if (vector->size*2 < vector->capacity && vector->size > VECTOR_START_CAP)
        vector_resize(vector, DOWN);
    size_t pos = real_pos(vector->capacity,(vector->start)++,0);
    void* retitem = vector->array[pos];
    --(vector->size);
    return retitem;
}

void* vector_pop_back(vector_t* vector) {
    if (vector->size == 0) {
        warning("vector_pop_front: Tried to pop an empty vector");
        return 0;
    }
    if (vector->size*2 < vector->capacity && vector->size > VECTOR_START_CAP)
        vector_resize(vector, DOWN);
    size_t pos = real_pos(vector->capacity,vector->start,vector->size-1);
    void* retitem = vector->array[pos];
    --(vector->size);
    return retitem;
}

void* vector_at(vector_t* vector, size_t pos) {
    size_t rpos = real_pos(vector->capacity, vector->start, pos);
    return vector->array[rpos];
}

void vector_resize(vector_t* vector, resize_t resize) {
    size_t old_capacity = vector->capacity;
    if (resize == UP && vector->capacity << 1 != 1)
        vector->capacity <<= 1;
    else
        vector->capacity >>= 1;
    if (vector->start < 0) {
        void** old_array = vector->array;
        vector->array = _ifj15_malloc(ARRAY, sizeof(void*)*vector->capacity, false);
        for (int i = 0; i < vector->size; ++i) {
            size_t pos = real_pos(old_capacity, vector->start, i);
            vector->array[i] = old_array[pos];
        }
        free(old_array);
        vector->start = 0;
    } else {
        vector->array = _ifj15_realloc(vector->array,
                                       sizeof(void*)*vector->capacity, false);
    }
}

size_t real_pos(size_t capacity, int start, size_t pos) {
    if (start < 0)
        // ANDed with capacity-1 which has the same effect as modulo when the
        // number is power of 2
        return (start+capacity+pos) & (capacity-1);
    else
        return start+pos;
}

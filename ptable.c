#include "ptable.h"

typedef enum {UP, DOWN} resize_t;
const size_t PTABLE_START_POW = 5;

void ptable_resize(ptable_t* ptable, resize_t resize);
static size_t hash(const void* key, size_t capacity_pow);
static unsigned int pow2(unsigned int pow);

ptable_t* _ptable_init(bool ptable_insert) {
    ptable_t* ptable = _ifj15_calloc(PTABLE, sizeof(ptable_t), ptable_insert);
    ptable->array = _ifj15_calloc(ARRAY, sizeof(ulist_t)*pow2(PTABLE_START_POW),
                                  false);
    ptable->capacity_pow = PTABLE_START_POW;
    ptable->size = 0;
    return ptable;
}

void _ptable_free(ptable_t* ptable) {
    size_t capacity = pow2(ptable->capacity_pow);
    for (size_t i = 0; i < capacity; ++i) {
        if (ptable->array[i] != NULL)
            _ulist_free(ptable->array[i]);
    }
    free(ptable->array);
    free(ptable);
}

void ptable_insert(ptable_t* ptable, void* ptr, ptr_t type) {
    ++(ptable->size);
    if (ptable->size > 10*pow2(ptable->capacity_pow))
        ptable_resize(ptable, UP);
    size_t hash_code = hash(ptr, ptable->capacity_pow);
    if (ptable->array[hash_code] == NULL)
        ptable->array[hash_code] = _ulist_init(false);
    ulist_set(ptable->array[hash_code], ptr, type);
}

ptr_t ptable_pop(ptable_t* ptable, void* ptr) {
    --(ptable->size);
    if (ptable->size < 2*pow2(ptable->capacity_pow)
        && ptable->capacity_pow > PTABLE_START_POW)
        ptable_resize(ptable, DOWN);
    size_t hash_code = hash(ptr, ptable->capacity_pow);
    if (ptable->array[hash_code] == NULL)
        error("ptable_pop: Failed to find searched for item", ERROR_INTERNAL);
    return ulist_pop(ptable->array[hash_code], ptr);
}

void ptable_resize(ptable_t* ptable, resize_t resize) {
    size_t old_capacity = pow2(ptable->capacity_pow);
    if (resize == UP)
        ptable->capacity_pow += 1;
    else
        ptable->capacity_pow -= 1;

    ulist_t** old_array = ptable->array;
    ptable->array = _ifj15_malloc(ARRAY, sizeof(ulist_t*)*pow2(ptable->capacity_pow), false);
    for (size_t i = 0; i < old_capacity; ++i) {
        if (old_array[i] != NULL) {
            unode_t* unode = old_array[i]->front;
            while (unode != NULL) {
                ptable_insert(ptable, unode->key, unode->item);
                unode = unode->next;
            }
            _ulist_free(old_array[i]);
        }
    }
    free(old_array);
}

static size_t hash(const void* key, size_t capacity_pow) {
    size_t hash;
#ifdef __x86_64__
    hash = ((unsigned long) key * 11400714819323199563UL) >> (64 - capacity_pow);
#else
    hash = ((unsigned long) key * 2654435761U) >> (32 - capacity_pow);
#endif
    return hash;
}

static unsigned int pow2(unsigned int pow) {
    return (1UL << pow);
}

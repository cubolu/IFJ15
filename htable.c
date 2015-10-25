#include "htable.h"

typedef enum {UP, DOWN} resize_t;
const size_t HTABLE_START_SIZE = 193;
// Prime numbers
size_t HTABLE_SIZES[24] = {193, 389, 769, 1543, 3079, 6151, 12289, 24593,
                           49157, 98317, 196613, 393241, 786433, 1572869,
                           3145739, 6291469, 12582917, 25165843, 50331653,
                           100663319, 201326611, 402653189, 805306457,
                           1610612741};

void htable_resize(htable_t* htable, resize_t resize);
static unsigned int hash(char* key, size_t cap_index);

htable_t* htable_init() {
    htable_t* htable = _ifj15_calloc(HTABLE, sizeof(htable_t), true);
    htable->array = _ifj15_calloc(ARRAY, sizeof(ulist_t)*HTABLE_START_SIZE,
                                  false);
    htable->cap_index = 0;
    htable->size = 0;
    return htable;
}

void _htable_free(htable_t* htable) {
    size_t capacity = HTABLE_SIZES[htable->cap_index];
    for (size_t i = 0; i < capacity; ++i) {
        if (htable->array[i] != NULL)
            _ulist_free(htable->array[i]);
    }
    free(htable->array);
    free(htable);
}

void htable_set(htable_t* htable, char* key, void* item) {
    ++(htable->size);
    if (htable->size > 5*HTABLE_SIZES[htable->cap_index])
        htable_resize(htable, UP);
    size_t hash_code = hash(key, htable->cap_index);
    if (htable->array[hash_code] == NULL)
        htable->array[hash_code] = _ulist_init(false);
    ulist_set(htable->array[hash_code], key, item);
}

void* htable_get(htable_t* htable, char* key) {
    size_t hash_code = hash(key, htable->cap_index);
    if (htable->array[hash_code] == NULL)
        return 0;
    return ulist_get(htable->array[hash_code], key);
}

void* htable_pop(htable_t* htable, char* key) {
    --(htable->size);
    if (htable->size < HTABLE_SIZES[htable->cap_index] && htable->cap_index > 0)
        htable_resize(htable, DOWN);
    size_t hash_code = hash(key, htable->cap_index);
    if (htable->array[hash_code] == NULL)
        return 0;
    return ulist_pop(htable->array[hash_code], key);
}

void htable_resize(htable_t* htable, resize_t resize) {
    size_t old_capacity = HTABLE_SIZES[htable->cap_index];
    if (resize == UP)
        ++htable->cap_index;
    else
        --htable->cap_index;

    ulist_t** old_array = htable->array;
    size_t capacity = HTABLE_SIZES[htable->cap_index];
    htable->array = _ifj15_malloc(ULIST, sizeof(ulist_t*)*capacity, false);
    for (size_t i = 0; i < old_capacity; ++i) {
        if (old_array[i] != NULL) {
            unode_t* unode = old_array[i]->front;
            while (unode != NULL) {
                htable_set(htable, (char*)unode->key, unode->item);
                unode = unode->next;
            }
            _ulist_free(old_array[i]);
        }
    }
    free(old_array);
}

static unsigned int hash(char* key, size_t cap_index) {
    size_t i = 0;
    int hash = 0;
    while (key[i] != 0) {
        hash = key[i] + (31 * hash);
        ++i;
    }
    return (hash & 0x7fffffff) % HTABLE_SIZES[cap_index];
}

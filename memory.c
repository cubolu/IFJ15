#include "memory.h"

void _ifj15_free(void* ptr, ptr_t ptr_type);
void ifj15_free_all();
static unsigned int pow2(unsigned int pow);

void ifj15_memory_init() {
    ifj15_ptable = _ptable_init(false);
}

void* _ifj15_malloc(ptr_t type, size_t size, bool ptable_insert_b) {
    void* addr = malloc(size);
    if (addr == NULL)
        error("Failed to allocate memory", ERROR_INTERNAL);
    if (ptable_insert_b)
        ptable_insert(ifj15_ptable, addr, type);
    return addr;
}

void* ifj15_realloc(void* ptr, size_t size) {
    void* addr = realloc(ptr, size);
    if (addr == NULL)
        error("Failed to reallocate memory", ERROR_INTERNAL);
    else if (addr != ptr) {
        ptr_t oldp_type = ptable_pop(ifj15_ptable, ptr);
        ptable_insert(ifj15_ptable, addr, oldp_type);
    }
    return addr;
}

void* _ifj15_calloc(ptr_t type, size_t size, bool ptable_insert_b) {
    void* addr = calloc(size, 1);
    if (addr == NULL)
        error("Failed to allocate memory", ERROR_INTERNAL);
    if (ptable_insert_b)
        ptable_insert(ifj15_ptable, addr, type);
    return addr;
}

void ifj15_free(void* ptr) {
    ptr_t ptr_type = ptable_pop(ifj15_ptable, ptr);
    _ifj15_free(ptr, ptr_type);
}

void _ifj15_free(void* ptr, ptr_t ptr_type) {
    switch(ptr_type) {
        case PTABLE:
            _ptable_free(ptr);
            break;
        case HTABLE:
            _htable_free(ptr);
            break;
        case LIST:
            _list_free(ptr);
            break;
        default:
            free(ptr);
    }
}

void ifj15_free_all() {
    size_t capacity = pow2(ifj15_ptable->capacity_pow);
    for (size_t i = 0; i < capacity; ++i) {
        if (ifj15_ptable->array[i] != NULL) {
            node_t* node = ifj15_ptable->array[i]->front;
            while (node != NULL) {
                _ifj15_free(node->key, (ptr_t)node->item);
                node = node->next;
            }
            _list_free(ifj15_ptable->array[i]);
        }
    }
    free(ifj15_ptable->array);
    free(ifj15_ptable);
}

static unsigned int pow2(unsigned int pow) {
    return (1UL << pow);
}

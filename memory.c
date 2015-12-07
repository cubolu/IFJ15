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

void* _ifj15_realloc(void* ptr, size_t size, bool ptable_insert_b) {
    void* addr = realloc(ptr, size);
    if (addr == NULL)
        error("Failed to reallocate memory", ERROR_INTERNAL);
    else if (addr != ptr && ptable_insert_b) {
        ptr_t oldp_type = ptable_pop(ifj15_ptable, ptr);
        ptable_insert(ifj15_ptable, addr, oldp_type);
    }
    return addr;
}

void* _ifj15_calloc(ptr_t type, size_t size, bool ptable_insert_b) {
    void* addr = calloc(1, size);
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
        case VECTOR:
            _vector_free(ptr);
            break;
        case STRING:
            _str_free(ptr);
            break;
        case SCANNER:
            _scanner_free(ptr);
            break;
        case FILEP:
            ifj15_fclose(ptr);
            break;
        case ULIST:
            _ulist_free(ptr);
            break;
        case ULISTSTR:
            _ulist_str_free(ptr);
            break;
        default:
            free(ptr);
    }
}

void ifj15_free_all() {
    size_t capacity = pow2(ifj15_ptable->capacity_pow);
    for (size_t i = 0; i < capacity; ++i) {
        if (ifj15_ptable->array[i] != NULL) {
            unode_t* unode = ifj15_ptable->array[i]->front;
            while (unode != NULL) {
                _ifj15_free(unode->key, (ptr_t)unode->item);
                unode = unode->next;
            }
            _ulist_free(ifj15_ptable->array[i]);
        }
    }
    free(ifj15_ptable->array);
    free(ifj15_ptable);
}

FILE* ifj15_fopen(const char* filename, const char* mod) {
    FILE* fp;
    fp = fopen(filename, mod);
    if (fp == NULL)
        error("Failed to open file", ERROR_INTERNAL);
    ptable_insert(ifj15_ptable, fp, FILEP);
    return fp;
}
void ifj15_fclose(FILE* fp) {
    ptable_pop(ifj15_ptable, fp);
    if (fclose(fp) == EOF)
        error("Failed to close file", ERROR_INTERNAL);
}

static unsigned int pow2(unsigned int pow) {
    return (1UL << pow);
}

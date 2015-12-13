#include "ulist.h"

unode_t* unode_init(void* key, ptr_t item);
unode_str_t* unode_str_init(str_t* key, symbol_t item);

ulist_t* _ulist_init(bool ptable_insert) {
    ulist_t* ulist = _ifj15_malloc(ULIST, sizeof(ulist_t), ptable_insert);
    ulist->front = NULL;
    ulist->back = NULL;
    return ulist;
}

ulist_str_t* _ulist_str_init(bool ptable_insert) {
    ulist_str_t* ulist = _ifj15_malloc(ULISTSTR, sizeof(ulist_str_t),
                                       ptable_insert);
    ulist->front = NULL;
    ulist->back = NULL;
    return ulist;
}

void _ulist_free(ulist_t* ulist) {
    unode_t* unode = ulist->front;
    unode_t* tmp;
    while (unode != NULL) {
        tmp = unode;
        unode = unode->next;
        free(tmp);
    }
    free(ulist);
}

void _ulist_str_free(ulist_str_t* ulist) {
    unode_str_t* unode = ulist->front;
    unode_str_t* tmp;
    while (unode != NULL) {
        tmp = unode;
        unode = unode->next;
        free(tmp);
    }
    free(ulist);
}

void _ulist_set(ulist_t* ulist, void* key, ptr_t item) {
    if (ulist->front == NULL) {
        unode_t* new_unode = unode_init(key, item);
        ulist->front = ulist->back = new_unode;
    } else {
        unode_t* unode = ulist->front;
        while (unode != NULL) {
            if (unode->key == key) {
                unode->item = item;
                return;
            }
            unode = unode->next;
        }
        // If no previous entry under key found
        unode_t* new_unode = unode_init(key, item);
        new_unode->next = ulist->front;
        ulist->front->prev = new_unode;
        ulist->front = new_unode;
    }
}

void _ulist_str_set(ulist_str_t* ulist, str_t* key, symbol_t item) {
    if (ulist->front == NULL) {
        unode_str_t* new_unode = unode_str_init(key, item);
        ulist->front = ulist->back = new_unode;
    } else {
        unode_str_t* unode = ulist->front;
        while (unode != NULL) {
            if (str_equal(unode->key, key)) {
                unode->item = item;
                return;
            }
            unode = unode->next;
        }
        // If no previous entry under key found
        unode_str_t* new_unode = unode_str_init(key, item);
        new_unode->next = ulist->front;
        ulist->front->prev = new_unode;
        ulist->front = new_unode;
    }
}

ptr_t _ulist_get(ulist_t* ulist, void* key) {
    unode_t* unode = ulist->front;
    ptr_t item;
    while (unode != NULL) {
        if (unode->key == key) {
            item = unode->item;
            return item;
        }
        unode = unode->next;
    }
    return -1;
}

symbol_t* _ulist_str_get(ulist_str_t* ulist, str_t* key) {
    unode_str_t* unode = ulist->front;
    symbol_t* item;
    while (unode != NULL) {
        if (str_equal(unode->key, key)) {
            item = &(unode->item);
            return item;
        }
        unode = unode->next;
    }
    return NULL;
}

ptr_t _ulist_pop(ulist_t* ulist, void* key) {
    unode_t* unode = ulist->front;
    ptr_t item;
    while (unode != NULL) {
        if (unode->key == key) {
            item = unode->item;
            if (unode->prev)
                unode->prev->next = unode->next;
            else // Front of the ulist
                ulist->front = unode->next;
            if (unode->next)
                unode->next->prev = unode->prev;
            else //Back of the ulist
                ulist->back = unode->prev;
            free(unode);
            return item;
        }
        unode = unode->next;
    }
    return -1;
}

symbol_t _ulist_str_pop(ulist_str_t* ulist, str_t* key) {
    unode_str_t* unode = ulist->front;
    symbol_t item;
    while (unode != NULL) {
        if (str_equal(unode->key, key)) {
            item = unode->item;
            if (unode->prev)
                unode->prev->next = unode->next;
            else // Front of the ulist
                ulist->front = unode->next;
            if (unode->next)
                unode->next->prev = unode->prev;
            else //Back of the ulist
                ulist->back = unode->prev;
            free(unode);
            return item;
        }
        unode = unode->next;
    }
    return INVALID_SYM;
}

unode_t* unode_init(void* key, ptr_t item) {
    unode_t* unode = _ifj15_malloc(SIMPLE, sizeof(unode_t), false);
    unode->key = key;
    unode->item = item;
    unode->next = NULL;
    unode->prev = NULL;
    return unode;
}

unode_str_t* unode_str_init(str_t* key, symbol_t item) {
    unode_str_t* unode = _ifj15_malloc(SIMPLE, sizeof(unode_str_t), false);
    unode->key = key;
    unode->item = item;
    unode->next = NULL;
    unode->prev = NULL;
    return unode;
}

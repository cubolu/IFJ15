#include "ulist.h"

bool equal(void* key1, void* key2, key_t key_type);
unode_t* unode_init(void* key, void* item);

ulist_t* _ulist_init(bool ptable_insert) {
    ulist_t* ulist = _ifj15_malloc(ULIST, sizeof(ulist_t), ptable_insert);
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

void _ulist_set(ulist_t* ulist, void* key, void* item, key_t key_type) {
    if (ulist->front == NULL) {
        unode_t* new_unode = unode_init(key, item);
        new_unode->next = ulist->front;
        ulist->front = ulist->back = new_unode;
    } else {
        unode_t* unode = ulist->front;
        while (unode != NULL) {
            if (equal(unode->key, key, key_type)) {
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

void* _ulist_get(ulist_t* ulist, void* key, key_t key_type, bool remove_item) {
    unode_t* unode = ulist->front;
    void* item;
    while (unode != NULL) {
        if (equal(unode->key, key, key_type)) {
            item = unode->item;
            if (remove_item) {
                if (unode->prev)
                    unode->prev->next = unode->next;
                else // Front of the ulist
                    ulist->front = unode->next;
                if (unode->next == NULL) //Back of the ulist
                    ulist->back = unode->prev;
                free(unode);
            }
            return item;
        }
        unode = unode->next;
    }
    return 0;
}

bool equal(void* key1, void* key2, key_t key_type) {
    switch (key_type) {
        case CHARARRAY:
            return strcmp(key1, key2) == 0;
        case POINTER:
            return key1 == key2;
    }
    error("equal: Invalid key type", ERROR_INTERNAL);
    return 0;
}

unode_t* unode_init(void* key, void* item) {
    unode_t* unode = _ifj15_malloc(SIMPLE, sizeof(unode_t), false);
    unode->key = key;
    unode->item = item;
    unode->next = NULL;
    unode->prev = NULL;
    return unode;
}

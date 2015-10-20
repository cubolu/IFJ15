#ifndef LIST_H
#define LIST_H

#include "common.h"
#include "error.h"
#include "memory.h"

typedef enum {POINTER, CHARARRAY} key_t;

struct _node_t {
	void* key;
	void* item;
	node_t* next;
	node_t* prev;
};

struct _list_t {
	node_t* front;
	node_t* back;
};

#define list_pop(list, key) _Generic((key), \
	char* : _list_pop(list, key, CHARARRAY), \
	default: _list_pop(list, key, POINTER))

#define list_init() _list_init(true)

list_t* _list_init(bool ptable_insert);
void list_free(list_t* list);

void list_insert(list_t* list, void* key, void* item);
void* _list_pop(list_t* list, void* key, key_t key_type);


#endif

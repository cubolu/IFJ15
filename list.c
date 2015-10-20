#include "list.h"

bool equal(void* key1, void* key2, key_t key_type);
node_t* node_init(void* key, void* item);

list_t* _list_init(bool ptable_insert) {
	list_t* list = _ifj15_malloc(LIST, sizeof(list_t), ptable_insert);
	list->front = NULL;
	list->back = NULL;
	return list;
}

void list_free(list_t* list) {
	node_t* node = list->front;
	node_t* tmp;
	while (node != NULL) {
		tmp = node;
		node = node->next;
		free(tmp);
	}
	free(list);
}

void list_insert(list_t* list, void* key, void* item) {
	node_t* new_node = node_init(key, item);
	new_node->item = item;
	if (list->front == NULL) {
		list->front = list->back = new_node;
	} else {
		new_node->next = list->front;
		list->front->prev = new_node;
		list->front = new_node;
	}
}

void* _list_pop(list_t* list, void* key, key_t key_type) {
	node_t* node = list->front;
	void* item;
	while (node != NULL) {
		if (equal(node->key, key, key_type)) {
			if (node->prev)
				node->prev->next = node->next;
			else // Front of the list
				list->front = node->next;
			if (node->next == NULL) //Back of the list
				list->back = node->prev;
			item = node->item;
			free(node);
			return item;
		}
		node = node->next;
	}
	error("list_pop: Failed to find searched for item", ERROR_INTERNAL);
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

node_t* node_init(void* key, void* item) {
	node_t* node = _ifj15_malloc(SIMPLE, sizeof(node_t), false);
    node->key = key;
    node->item = item;
	node->next = NULL;
	node->prev = NULL;
	return node;
}

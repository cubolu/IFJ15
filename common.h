#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "vfunc.h"

// Forward declarations that break cyclic dependencies
typedef struct _unode_t unode_t;
typedef struct _unode_str_t unode_str_t;
typedef struct _ulist_t ulist_t;
typedef struct _ulist_str_t ulist_str_t;
typedef struct _vector_t vector_t;
typedef struct _stack_char_t stack_char_t;
typedef struct _stack_token_t stack_token_t;
typedef struct _stack_htable_t stack_htable_t;
typedef struct _ptable_t ptable_t;
typedef struct _htable_t htable_t;
typedef struct _str_t str_t;
typedef struct _token_t token_t;
typedef struct _symbol_t symbol_t;

typedef enum {PTABLE, HTABLE, VECTOR, ARRAY, SIMPLE, STACK, STRING, SCANNER, FILEP} ptr_t;

typedef bool (*stack_compare)(char stack_item);

#endif

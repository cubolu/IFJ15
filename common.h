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
typedef struct _vector_char_t vector_char_t;
typedef struct _vector_int_t vector_int_t;
typedef struct _vector_token_t vector_token_t;
typedef struct _vector_htable_t vector_htable_t;
//typedef struct _vector_void_t vector_void_t;
typedef struct _ptable_t ptable_t;
typedef struct _htable_t htable_t;
typedef struct _str_t str_t;
typedef struct _token_t token_t;
typedef struct _symbol_t symbol_t;

typedef bool (*vector_compare)(char vector_item);

#endif

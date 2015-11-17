#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "vfunc.h"

// Forward declarations that break cyclic dependencies
typedef struct _unode_t unode_t;
typedef struct _ulist_t ulist_t;
typedef struct _vector_t vector_t;
typedef struct _ptable_t ptable_t;
typedef struct _htable_t htable_t;
typedef struct _str_t str_t;
typedef enum _ptr_t ptr_t;

#endif

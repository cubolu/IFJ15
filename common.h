#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "vfunc.h"

// Forward declarations that break cyclic dependencies
typedef struct _node_t node_t;
typedef struct _list_t list_t;
typedef struct _ptable_t ptable_t;
typedef struct _htable_t htable_t;
typedef enum _ptr_t ptr_t;

#endif

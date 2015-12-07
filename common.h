#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "vfunc.h"

// Forward declarations that break cyclic dependencies
typedef struct _unode_t unode_t;
typedef struct _unode_str_t unode_str_t;
typedef struct _ulist_t ulist_t;
typedef struct _ulist_str_t ulist_str_t;
typedef struct _vector_char_t vector_char_t;
typedef struct _vector_int_t vector_int_t;
typedef struct _vector_expr_t vector_expr_t;
typedef struct _vector_htable_t vector_htable_t;
typedef struct _vector_token_t vector_token_t;
typedef struct _vector_inst_t vector_inst_t;
typedef struct _vector_data_seg_t vector_data_seg_t;
typedef struct _ptable_t ptable_t;
typedef struct _htable_t htable_t;
typedef struct _str_t str_t;
typedef struct _token_t token_t;
typedef struct _symbol_t symbol_t;
typedef struct _func_call_t func_call_t;
typedef struct _expression_t expression_t;
typedef struct _inst_t inst_t;
typedef union _data_seg_t data_seg_t;

typedef bool (*vector_compare)(char vector_item);

// NDEBUG disable the debug mode
#ifdef NDEBUG

#define dLineNumInit(scanner)
#define dAddLineNum(inst_name)
//...

// DEBUG MODE
#else

//set line number stream
#define dLineNumInit(scanner) line_stream = scanner
//add line numer to instruction in code segment
#define dAddLineNum(inst_name) inst_name.line = line_stream->line

#endif //end NDEBUG

#endif

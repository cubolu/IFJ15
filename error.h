#ifndef ERROR_H
#define ERROR_H

#include "common.h"
#include "memory.h"

typedef enum _error_t {ERROR_LEX=1, ERROR_SYN, ERROR_SEM, ERROR_TYPE_COMPAT,
                       ERROR_TYPE_DEDUCTION, ERROR_SEM_OTHER, ERROR_NUM_INPUT,
                       ERROR_UNDEF, ERROR_DIVZERO, ERROR_RUNTIME,
                       ERROR_INTERNAL=99} error_t;

void error(const char *error_str, error_t error_type);
void warning(const char *warn_str);

#endif

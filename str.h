#ifndef STR_H
#define STR_H

#include "common.h"

#define STRING_SIZE_INC 8

typedef struct {
    char * str;
    size_t curr_len;
    size_t mem_size;
} str;

str * str_init(void);

void _str_free(str * s);

void str_append_char(str * s, char c);

bool str_equals(str * s, char * s2);

#endif // STR_H


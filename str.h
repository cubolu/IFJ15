#ifndef STR_H
#define STR_H

#include "common.h"
#include "memory.h"

struct _str_t {
    char * str;
    size_t length;
    size_t capacity;
};

str_t * str_init(void);

void str_free(str_t * s);

void str_append_char(str_t * s, char c);

bool str_equals(str_t * s, char * s2);

double str_to_double(str_t *s);

int str_to_int(str_t *s);

void str_clear(str_t *s);

char str_last_char(str_t *s);

#endif // STR_H

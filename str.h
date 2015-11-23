#ifndef STR_H
#define STR_H

#include "common.h"

struct _str_t {
    char * c_str;
    size_t length;
    size_t capacity;
};

#include "memory.h"

#define str_init() _str_init(true)

#define str_equal(s1, s2) _Generic((s1),   \
    char*: _Generic((s2),                   \
        char*: _str_eq_char_char,           \
        str_t*: _str_eq_char_str),          \
    str_t*: _Generic((s2),                  \
        char*: _str_eq_str_char,            \
        str_t*: _star_eq_str_str))(s1,s2)

str_t * _str_init(bool ptable_insert);
void _str_free(str_t * s);

void str_append_char(str_t * s, char c);
void str_copy(str_t* dest, const char* source);
bool _str_eq_char_char(char * s1, char * s2);
bool _str_eq_char_str(char * s1, str_t * s2);
bool _str_eq_str_char(str_t * s1, char * s2);
bool _star_eq_str_str(str_t * s1, str_t * s2);
double str_to_double(str_t *s);
int str_to_int(str_t *s);
void str_clear(str_t *s);
char str_last_char(str_t *s);

#endif // STR_H

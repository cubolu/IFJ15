#include "str.h"

const size_t STR_START_CAP = 8;

str_t * _str_init(bool ptable_insert) {
    str_t * s;
    s = _ifj15_malloc(STRING, sizeof(str_t), ptable_insert);
    s->c_str = _ifj15_malloc(ARRAY, STR_START_CAP, false);
    s->length = 0;
    s->capacity = STR_START_CAP;
    s->c_str[0] = '\0';
    return s;
}

void _str_free(str_t* s) {
    free(s->c_str);
    free(s);
}


void _str_double_size(str_t * s) {
    size_t newcap = 2*s->capacity;
    s->c_str = _ifj15_realloc(s->c_str, newcap, false);
    s->capacity = newcap;
}

void _str_resize(str_t* s, size_t size) {
    s->c_str = _ifj15_realloc(s->c_str, size, false);
    s->capacity = size;
}

void str_append_char(str_t * s, char c) {
    if (s->length + 2 > s->capacity) // need space for c and ending \0
        _str_double_size(s);
    s->c_str[s->length] = c;
    s->c_str[++s->length] = '\0';
}

void str_copy(str_t* dest, const char* source) {
    size_t s_len = strlen(source);
    _str_resize(dest, s_len+1);
    strcpy(dest->c_str, source);
    dest->length = s_len;
}

bool _str_eq_char_char(char * s1, char * s2) {
    return strcmp(s1, s2) == 0;
}
bool _str_eq_char_str(char * s1, str_t * s2) {
    return strcmp(s1, s2->c_str) == 0;
}
bool _str_eq_str_char(str_t * s1, char * s2) {
    return strcmp(s1->c_str, s2) == 0;
}
bool _star_eq_str_str(str_t * s1, str_t * s2) {
    return strcmp(s1->c_str, s2->c_str) == 0;
}

// predpokladam, ze nemuze dojit k chybe pri prevodu, protoze format byl overen konecnym automatem
double str_to_double(str_t *s) {
    return strtod(s->c_str, NULL);
}

// predpokladam, ze nemuze dojit k chybe pri prevodu, protoze format byl overen konecnym automatem
int str_to_int(str_t *s) {
    return strtol(s->c_str, NULL, 10);
}

void str_clear(str_t *s) {
    if (s->length == 0)
        return;

    s->c_str = _ifj15_realloc(s->c_str, STR_START_CAP, false);
    s->capacity = STR_START_CAP;
    s->length  = 0;
    s->c_str[0] = '\0';
}

char str_last_char(str_t *s) {
    if (s->length == 0)
        return 0;

    else
        return s->c_str[s->length -1];
}

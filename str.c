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


void _str_rise(str_t * s) {
    size_t newcap = 2*s->capacity;
    s->c_str = _ifj15_realloc(s->c_str, newcap, false);
    s->capacity = newcap;
}

void str_append_char(str_t * s, char c) {
    if (s->length + 2 > s->capacity) // need space for c and ending \0
        _str_rise(s);
    s->c_str[s->length] = c;
    s->c_str[++s->length] = '\0';
}

bool str_equals(str_t * s, char * s2) {
    return !(strcmp(s->c_str, s2));
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

    //TODO osetrit selhani reallocu
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

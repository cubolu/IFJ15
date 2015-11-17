#include "str.h"

const size_t STR_START_CAP = 8;

str_t * str_init(void) {
    str_t * s;
    s = ifj15_malloc(STRING, sizeof(str_t));
    s->str = _ifj15_malloc(SIMPLE, STR_START_CAP, false);
    s->length = 0;
    s->capacity = STR_START_CAP;
    s->str[0] = '\0';
    return s;
}

void str_free(str_t * s) {
    free(s->str);
    free(s);
}


void _str_rise(str_t * s) {
    size_t newcap = 2*s->capacity;
    s->str = _ifj15_realloc(s->str, newcap, false);
    s->capacity = newcap;
}

void str_append_char(str_t * s, char c) {
    if (s->length + 2 > s->capacity) // need space for c and ending \0
        _str_rise(s);
    s->str[s->length] = c;
    s->str[++s->length] = '\0';
}

bool str_equals(str_t * s, char * s2) {
    return !(strcmp(s->str, s2));
}

// predpokladam, ze nemuze dojit k chybe pri prevodu, protoze format byl overen konecnym automatem
double str_to_double(str_t *s) {
    return strtod(s->str, NULL);
}

// predpokladam, ze nemuze dojit k chybe pri prevodu, protoze format byl overen konecnym automatem
int str_to_int(str_t *s) {
    return strtol(s->str, NULL, 10);
}

void str_clear(str_t *s) {
    if (s->length == 0)
        return;

    //TODO osetrit selhani reallocu
    s->str = _ifj15_realloc(s->str, STR_START_CAP, false);
    s->capacity = STR_START_CAP;
    s->length  = 0;
    s->str[0] = '\0';
}

char str_last_char(str_t *s) {
    if (s->length == 0)
        return 0;

    else
        return s->str[s->length -1];
}

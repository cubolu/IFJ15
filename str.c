#include "str.h"
#include "memory.h"

str * str_init(void)
{
    str * s;
    s = _ifj15_malloc(STRING, sizeof (str), true);
    s->str = malloc(STRING_SIZE_INC);
    s->curr_len = 0;
    s->mem_size = STRING_SIZE_INC;

    s->str[0] = '\0';

    return s;
}

void str_free(str * s)
{
    free(s->str);
    free(s);
}


void _str_rise(str * s)
{
    size_t newsize = s->mem_size + STRING_SIZE_INC;
    //TODO osetrit selhani reallocu
    s->str = realloc(s->str, newsize);
    s->mem_size = newsize;
}

void str_append_char(str * s, char c)
{
    if (s->curr_len + 2 > s->mem_size) // need space for c and ending \0
        _str_rise(s);

    s->str[s->curr_len] = c;

    s->str[++s->curr_len] = '\0';
}

bool str_equals(str * s, char * s2)
{
    return !(strcmp(s->str, s2));
}

// predpokladam, ze nemuze dojit k chybe pri prevodu, protoze format byl overen konecnym automatem
double str_to_double(str *s)
{
    return strtod(s->str, NULL);
}

// predpokladam, ze nemuze dojit k chybe pri prevodu, protoze format byl overen konecnym automatem
int str_to_int(str *s)
{
    return strtol(s->str, NULL, 10);
}

void str_clear(str *s)
{
    //TODO osetrit selhani reallocu
    s->str = realloc(s->str, STRING_SIZE_INC);
    s->mem_size = STRING_SIZE_INC;
    s->curr_len = 0;
    s->str[0] = '\0';
}

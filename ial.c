#include "ial.h"
#include "str.h"

int length(str_t * s)
{
    return s->length;
}

str_t * substr(str_t * s, int i, int n)
{
    if (i > s->length)
        return NULL;

    str_t * ret = str_init();

    if (n+i > s->length)
        n = s->length-i;

    _str_resize(ret, n+1);
    memcpy(ret->c_str, s->c_str+i, n);
    ret->c_str[n] = '\0';
    ret->length = n;

    return ret;
}

str_t * concat(str_t * s1, str_t * s2)
{
    str_t * ret = str_init();

    _str_resize(ret, s1->length+s2->length+1);

    memcpy(ret->c_str           , s1->c_str, s1->length);
    memcpy(ret->c_str+s1->length, s2->c_str, s2->length);

    ret->c_str[s1->length+s2->length] = '\0';
    ret->length = s1->length + s2->length;

    return ret;
}

int find(str_t * s, str_t * search);

str_t * sort(str_t * s);


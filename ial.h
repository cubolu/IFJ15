#ifndef IAL_H
#define IAL_H

#include "common.h"

int length(str_t * s);

str_t * substr(str_t * s, int i, int n);

str_t * concat(str_t * s1, str_t * s2);

int find(str_t * s, str_t * search);

str_t * sort(str_t * s);

#endif // IAL_H


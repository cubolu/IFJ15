#ifndef SCANNER_H
#define SCANNER_H

#include "str.h"
#include "token.h"

typedef struct {
    FILE* file;
    str_t* str;
    size_t line;
} scanner_t;


scanner_t* scanner_init(FILE* fp);
void _scanner_free(scanner_t* s);

token_t get_next_token(scanner_t* s);


#endif // SCANNER_H

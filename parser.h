#ifndef PARSER_H
#define PARSER_H

#include "token.h"

typedef struct {
    FILE * file;
    str * s;
    size_t line;
} parser;


parser * parser_init(char * filename);

token_t parser_next_token(parser * p);


#endif // PARSER_H


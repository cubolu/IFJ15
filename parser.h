#ifndef PARSER_H
#define PARSER_H

typedef struct {
    FILE * file;
} parser;


parser * parser_init(char * filename);

int parser_next_token(parser * p);


#endif // PARSER_H


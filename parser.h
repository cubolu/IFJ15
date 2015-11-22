#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include "token.h"
#include "scanner.h"
#include "stack.h"

void parse_program(scanner_t* s);

#endif

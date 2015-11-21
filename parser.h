#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "scanner.h"
#include "stack.h"

void parse_program(scanner_t* s);

//precedence table operators
typedef enum _tab_op_t {TAB_SP, TAB_P, TAB_R, TAB_END, TAB_ERR} tab_op_t;
//symbols for pushdown automata
typedef enum _stack_sym {S_REL=0, S_ADDSUB, S_MULDIV, S_PAR_O, S_PAR_C,
                         S_ID, S_END, S_SEP, S_EXPR} stack_sym_t;
//rules to parse an expression, note: rules are used contrariwise
#define RULE_1    ((S_EXPR << 16) | (S_REL << 8) | S_EXPR)    //E -> E <>= E
#define RULE_2    ((S_EXPR << 16) | (S_ADDSUB << 8) | S_EXPR) //E -> E +(-) E
#define RULE_3    ((S_EXPR << 16) | (S_MULDIV << 8) | S_EXPR) //E -> E *(/) E
#define RULE_4    ((S_PAR_C << 16) | (S_EXPR << 8) | S_PAR_O) //E -> ( E )
#define RULE_5    (S_ID) //E -> id

#endif

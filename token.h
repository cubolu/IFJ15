#ifndef TOKEN_H
#define TOKEN_H

#include "str.h"

enum e_token_type {
    TT_VARIABLE_DECLARATION,
    TT_SEMICOLON,
    TT_ASSIGNMENT,

    TT_DOUBLE,
    TT_INTEGER,
    TT_STRING,

    TT_BLOCK_START,
    TT_BLOCK_END,

    TT_IF,
    TT_ELSE,
    TT_FOR,

    TT_FUNCTION_CALL,
    TT_FUNCTION_DECLARATION,

    TT_OPERATOR,

    TT_RETURN,

    TT_EOF,
};

enum e_operator {
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_ADD,
    OP_SUBTRACT,
    OP_LESS,
    OP_GREATER,
    OP_LESS_OR_EQ,
    OP_GREATER_OR_EQ,
    OP_EQUAL,
    OP_NOT_EQUAL
};

enum e_variable_type {VT_NOT_A_TYPE = 0,
                      VT_INT,
                      VT_STRING,
                      VT_DOUBLE,
                      VT_AUTO};


typedef struct {
    enum e_variable_type t;
    str * name;
} token_var_declaration_t;

typedef struct {
    enum e_token_type type;
    union {
        token_var_declaration_t var_or_func_declaration;

        struct {
            double value;
        } double_num;

        struct {
            int value;
        } integer;

        struct {
            str * text;
        } string;

        struct {
            enum e_operator type;
        } op;


        //if, else, for
        //deklarace funkce
        //volani funkce
    };

} token_t;

#endif // TOKEN_H


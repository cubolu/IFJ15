#ifndef TOKEN_H
#define TOKEN_H

#include "common.h"

enum e_token_t {
    TT_NONE = 0,
    TT_KW_CIN,
    TT_KW_COUT,
    TT_KW_RETURN,
    TT_KW_FOR,
    TT_KW_IF,
    TT_KW_ELSE,

    TT_IDENTIFICATOR,

    TT_OP_ASSIGNMENT,
    TT_OP_RELATIONAL,
    TT_OP_ARITHMETIC,
    TT_OP_STREAM_IN,  // >>
    TT_OP_STREAM_OUT, // <<

    TT_TYPE_DOUBLE = 13,
    TT_TYPE_INT = 14,
    TT_TYPE_STRING = 15,
    TT_TYPE_AUTO,

    TT_BLOCK_START,
    TT_BLOCK_END,

    TT_PARENTHESES_OPEN,
    TT_PARENTHESES_CLOSE,

    TT_LIT_DOUBLE = 21,
    TT_LIT_INT,
    TT_LIT_STRING,

    TT_SEMICOLON,
    TT_COMMA,
    TT_EOF,
};

typedef enum {
    OP_ARITH_MULTIPLY = 0,
    OP_ARITH_DIVIDE,
    OP_ARITH_ADD,
    OP_ARITH_SUBTRACT = 3,
} op_arithmetic_t;

typedef enum {
    OP_REL_LESS = 4,
    OP_REL_GREATER,
    OP_REL_LESS_OR_EQ,
    OP_REL_GREATER_OR_EQ,
    OP_REL_EQUAL,
    OP_REL_NOT_EQUAL = 9
} op_relational_t;

#include "str.h"

struct _token_t {
    enum e_token_t type;
    union {
        op_arithmetic_t op_arith;
        op_relational_t op_rel;
        str_t * str;

        int int_val;
        double double_val;
    };

};

#endif // TOKEN_H

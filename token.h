#ifndef TOKEN_H
#define TOKEN_H

#include "str.h"

enum e_token_type {
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
    TT_OP_ARITHEMTIC,
    TT_OP_STREAM_IN,  // >>
    TT_OP_STREAM_OUT, // <<

    TT_TYPE_DOUBLE,
    TT_TYPE_INT,
    TT_TYPE_STRING,
    TT_TYPE_AUTO,

    TT_BLOCK_START,
    TT_BLOCK_END,

    TT_PARENTHESES_OPEN,
    TT_PARENTHESES_CLOSE,

    TT_LIT_INT,
    TT_LIT_DOUBLE,
    TT_LIT_STRING,

    TT_SEMICOLON,
    TT_EOF,
};

enum e_op_arithmetic {
    OP_ARITH_MULTIPLY,
    OP_ARITH_DIVIDE,
    OP_ARITH_ADD,
    OP_ARITH_SUBTRACT,
};

enum e_op_relational {
    OP_REL_LESS,
    OP_REL_GREATER,
    OP_REL_LESS_OR_EQ,
    OP_REL_GREATER_OR_EQ,
    OP_REL_EQUAL,
    OP_REL_NOT_EQUAL
};


typedef struct {
    enum e_token_type type;
    union {
        enum e_op_arithmetic op_arith;
        enum e_op_relational op_rel;
        str * s;

        int int_val;
        double double_val;
    };

} token_t;

#endif // TOKEN_H


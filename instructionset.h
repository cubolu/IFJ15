#ifndef INSTRUCTION_SET_H
#define INSTRUCTION_SET_H

#include "common.h"

//operations offsets
#define D_D_OFFSET  0
#define D_DL_OFFSET 10
#define DL_D_OFFSET 20
#define I_I_OFFSET  30
#define I_IL_OFFSET 40
#define IL_I_OFFSET 50
#define S_S_OFFSET  64
#define S_SL_OFFSET 74
#define SL_S_OFFSET 84

typedef enum {
    // DOUBLE
    INST_MUL_D_D = D_D_OFFSET,
    INST_DIV_D_D,
    INST_ADD_D_D,
    INST_SUB_D_D,
    INST_LESS_D_D,
    INST_GREATER_D_D,
    INST_LESS_OR_EQ_D_D,
    INST_GREATER_OR_EQ_D_D,
    INST_EQUAL_D_D,
    INST_NOT_EQUAL_D_D,

    INST_MUL_D_DL = D_DL_OFFSET,
    INST_DIV_D_DL,
    INST_ADD_D_DL,
    INST_SUB_D_DL,
    INST_LESS_D_DL,
    INST_GREATER_D_DL,
    INST_LESS_OR_EQ_D_DL,
    INST_GREATER_OR_EQ_D_DL,
    INST_EQUAL_D_DL,
    INST_NOT_EQUAL_D_DL,

    INST_MUL_DL_D = DL_D_OFFSET,
    INST_DIV_DL_D,
    INST_ADD_DL_D,
    INST_SUB_DL_D,
    INST_LESS_DL_D,
    INST_GREATER_DL_D,
    INST_LESS_OR_EQ_DL_D,
    INST_GREATER_OR_EQ_DL_D,
    INST_EQUAL_DL_D,
    INST_NOT_EQUAL_DL_D,
    // INT
    INST_MUL_I_I = I_I_OFFSET,
    INST_DIV_I_I,
    INST_ADD_I_I,
    INST_SUB_I_I,
    INST_LESS_I_I,
    INST_GREATER_I_I,
    INST_LESS_OR_EQ_I_I,
    INST_GREATER_OR_EQ_I_I,
    INST_EQUAL_I_I,
    INST_NOT_EQUAL_I_I,

    INST_MUL_I_IL = I_IL_OFFSET,
    INST_DIV_I_IL,
    INST_ADD_I_IL,
    INST_SUB_I_IL,
    INST_LESS_I_IL,
    INST_GREATER_I_IL,
    INST_LESS_OR_EQ_I_IL,
    INST_GREATER_OR_EQ_I_IL,
    INST_EQUAL_I_IL,
    INST_NOT_EQUAL_I_IL,

    INST_MUL_IL_I = IL_I_OFFSET,
    INST_DIV_IL_I,
    INST_ADD_IL_I,
    INST_SUB_IL_I,
    INST_LESS_IL_I,
    INST_GREATER_IL_I,
    INST_LESS_OR_EQ_IL_I,
    INST_GREATER_OR_EQ_IL_I,
    INST_EQUAL_IL_I,
    INST_NOT_EQUAL_IL_I,
    // STRING
    INST_LESS_S_S = S_S_OFFSET,
    INST_GREATER_S_S,
    INST_LESS_OR_EQ_S_S,
    INST_GREATER_OR_EQ_S_S,
    INST_EQUAL_S_S,
    INST_NOT_EQUAL_S_S,

    INST_LESS_S_SL = S_SL_OFFSET,
    INST_GREATER_S_SL,
    INST_LESS_OR_EQ_S_SL,
    INST_GREATER_OR_EQ_S_SL,
    INST_EQUAL_S_SL,
    INST_NOT_EQUAL_S_SL,

    INST_LESS_SL_S = SL_S_OFFSET,
    INST_GREATER_SL_S,
    INST_LESS_OR_EQ_SL_S,
    INST_GREATER_OR_EQ_SL_S,
    INST_EQUAL_SL_S,
    INST_NOT_EQUAL_SL_S,

    INST_PUSH,
    INST_CVT_INT,
    INST_HALT,
} inst_set_t;

struct _inst_t {
    inst_set_t inst_code;
    union {
        size_t op1_addr;
        str_t* op1_str_val;
        int op1_int_val;
        double op1_double_val;
    };
    union {
        size_t op2_addr;
        str_t* op2_str_val;
        int op2_int_val;
        double op2_double_val;
    };
    size_t res_addr;
};

union _data_seg_t {
    str_t* str_val;
    int int_val;
    double double_val;
    size_t ret_addr;
};

#endif

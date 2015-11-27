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
#define S_S_OFFSET  60
#define S_SL_OFFSET 70
#define SL_S_OFFSET 80

typedef enum {
    INST_MUL_D_D = D_D_OFFSET,
    INST_DIV_D_D,
    INST_ADD_D_D,
    INST_SUB_D_D,
    INST_LESS_D_D,
    INST_GREATER_D_D,
    INST_MUL_D_DL = D_DL_OFFSET,

    INST_PUSH = 90,
    INST_CVT,
    INST_HALT,
} inst_set_t; //TODO: add another instructions

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

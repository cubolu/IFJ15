#ifndef GENERATOR_H
#define GENERATOR_H

#include "common.h"
#include "instructionset.h"
#include "token.h"
#include "symbol.h"

extern vector_inst_t* code_seg;

//instruction set generators for IFJ15 interpreter
void generator_init();

void init_new_stack_frame();
size_t generate_push();
size_t generate_int_cnvrt();

size_t generate_D_D(int oper, \
    expression_t* op_left, expression_t* op_right, \
    bool op_l_convert, bool op_r_convert);
size_t generate_D_DL(int oper, \
    expression_t* op_left, expression_t* op_right, \
    bool op_l_convert, bool op_r_convert);
size_t generate_DL_D(int oper, \
    expression_t* op_left, expression_t* op_right, \
    bool op_l_convert, bool op_r_convert);
size_t generate_I_I(int oper, \
    expression_t* op_left, expression_t* op_right);
size_t generate_I_IL(int oper, \
    expression_t* op_left, expression_t* op_right);
size_t generate_IL_I(int oper, \
    expression_t* op_left, expression_t* op_right);
size_t generate_S_S(int oper, \
    expression_t* op_left, expression_t* op_right);
size_t generate_S_SL(int oper, \
    expression_t* op_left, expression_t* op_right);
size_t generate_SL_S(int oper, \
    expression_t* op_left, expression_t* op_right);

//literal solving functions
int solve_rel_DL_DL(op_relational_t oper, \
    expression_t* op_left, expression_t* op_right, \
    bool op_l_convert, bool op_r_convert);
int solve_rel_IL_IL(op_relational_t oper, \
    expression_t* op_left, expression_t* op_right);
int solve_rel_SL_SL(op_relational_t oper, \
    expression_t* op_left, expression_t* op_right);
double solve_arith_DL_DL(op_arithmetic_t oper, \
    expression_t* op_left, expression_t* op_right, \
    bool op_l_convert, bool op_r_convert);
int solve_arith_IL_IL(op_arithmetic_t oper, \
    expression_t* op_left, expression_t* op_right);

#define return_rel(oper, op1, op2) switch (oper) {      \
    case OP_REL_LESS:                                   \
        return op1 < op2;                               \
    case OP_REL_GREATER:                                \
        return op1 > op2;                               \
    case OP_REL_LESS_OR_EQ:                             \
        return op1 <= op2;                              \
    case OP_REL_GREATER_OR_EQ:                          \
        return op1 >= op2;                              \
    case OP_REL_EQUAL:                                  \
        return op1 == op2;                              \
    case OP_REL_NOT_EQUAL:                              \
        return op1 != op2;                              \
    default:                                            \
        return -1;                                      \
}

#define return_arith(oper, op1, op2) switch (oper) {    \
    case OP_ARITH_MULTIPLY:                             \
        return op1 * op2;                               \
    case OP_ARITH_DIVIDE:                               \
        return op1 / op2;                               \
    case OP_ARITH_ADD:                                  \
        return op1 + op2;                               \
    case OP_ARITH_SUBTRACT:                             \
        return op1 - op2;                               \
    default:                                            \
        return -1;                                      \
}

#endif

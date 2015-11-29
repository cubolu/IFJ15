#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "common.h"
#include "instruction.h"
#include "generator.h"
#include "vector.h"
#include "ial.h"

void interpreter_init();
void run_program();

//executable relational and arithmetic operation code:
#define OPER_ADDR_ADDR(oper, data_type, res_type)                           \
    res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);  \
    op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);   \
    op_right = vector_at(data_seg, inst->op2_addr + curr_stack_frame_ptr);  \
    res_addr->res_type = op_left->data_type oper op_right->data_type

#define OPER_ADDR_LIT(oper, data_type, res_type)                            \
    res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);  \
    op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);   \
    res_addr->res_type = op_left->data_type oper inst->op2_##data_type

#define OPER_LIT_ADDR(oper, data_type, res_type)                            \
    res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);  \
    op_right = vector_at(data_seg, inst->op2_addr + curr_stack_frame_ptr);  \
    res_addr->res_type = inst->op1_##data_type oper op_right->data_type

#define EXECSTR_ADDR_ADDR(func_name)                                        \
    res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);  \
    op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);   \
    op_right = vector_at(data_seg, inst->op2_addr);                         \
    res_addr->int_val = func_name(op_left->str_val, op_right->str_val)

#define EXECSTR_ADDR_LIT(func_name)                                         \
    res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);  \
    op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);   \
    res_addr->int_val = func_name(op_left->str_val, inst->op2_str_val)

#define EXECSTR_LIT_ADDR(func_name)                                         \
    res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);  \
    op_right = vector_at(data_seg, inst->op2_addr + curr_stack_frame_ptr);  \
    res_addr->int_val = func_name(inst->op1_str_val, op_right->str_val)

#endif

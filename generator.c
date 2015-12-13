#include "generator.h"

size_t curr_stack_frame;
vector_inst_t* code_seg;

void generator_init() {
    code_seg = vector_init(VI_CODESEG);
    //initialize built in functions
    inst_t curr_inst;
    //function address is 0
    curr_inst.inst_code = INST_BUILTIN_LENGTH;
    vector_push(code_seg, curr_inst);
    curr_inst.inst_code = INST_RESTORE;
    curr_inst.op1_addr = 1;
    vector_push(code_seg, curr_inst);

    //function address is 2
    curr_inst.inst_code = INST_BUILTIN_SUBSTR;
    vector_push(code_seg, curr_inst);
    curr_inst.inst_code = INST_RESTORE;
    curr_inst.op1_addr = 3;
    vector_push(code_seg, curr_inst);

    //function address is 4
    curr_inst.inst_code = INST_BUILTIN_CONCAT;
    vector_push(code_seg, curr_inst);
    curr_inst.inst_code = INST_RESTORE;
    curr_inst.op1_addr = 2;
    vector_push(code_seg, curr_inst);

    //function address is 6
    curr_inst.inst_code = INST_BUILTIN_FIND;
    vector_push(code_seg, curr_inst);
    curr_inst.inst_code = INST_RESTORE;
    curr_inst.op1_addr = 2;
    vector_push(code_seg, curr_inst);

    //function address is 8
    curr_inst.inst_code = INST_BUILTIN_SORT;
    vector_push(code_seg, curr_inst);
    curr_inst.inst_code = INST_RESTORE;
    curr_inst.op1_addr = 1;
    vector_push(code_seg, curr_inst);
}

size_t get_code_seg_top() {
    return code_seg->size;
}

void init_new_stack_frame(size_t param_count) {
    curr_stack_frame = param_count + 1; //index 0 is reseved for return address
    curr_stack_frame += 3; //data for INST_RESTORE
}

size_t store_stack_frame() {
    return curr_stack_frame;
}

void load_stack_frame(size_t stack_frame_value) {
    curr_stack_frame = stack_frame_value;
}

void generate_data_seg_restore(size_t restore_val) {
    inst_t curr_inst = {.inst_code = INST_DATA_SEGMENT_RESTORE};
    curr_inst.op1_addr = restore_val;
    vector_push(code_seg, curr_inst);
}

size_t generate_push() {
    inst_t curr_inst = {.inst_code = INST_PUSH};
    //DEBUG
    curr_inst.res_addr = curr_stack_frame;
    vector_push(code_seg, curr_inst);
    return curr_stack_frame++;
}

void generate_param_push(size_t source) {
    inst_t curr_inst = {.inst_code = INST_PUSH_VALUE};
    curr_inst.op1_addr = source;
    vector_push(code_seg, curr_inst);
}

void generate_param_conv_push(size_t source, bool to_int, bool to_double) {
    inst_t curr_inst = {.inst_code = INST_PUSH_VALUE};
    if (to_int)
        curr_inst.inst_code = INST_CONV_TO_INT_AND_PUSH_VALUE;
    else if (to_double)
        curr_inst.inst_code = INST_CONV_TO_DOUBLE_AND_PUSH_VALUE;
    curr_inst.op1_addr = source;
    vector_push(code_seg, curr_inst);
}

void generate_param_push_double(double val) {
    inst_t curr_inst = {.inst_code = INST_PUSH_DOUBLE};
    curr_inst.op1_double_val = val;
    vector_push(code_seg, curr_inst);
}

void generate_param_push_int(int val) {
    inst_t curr_inst = {.inst_code = INST_PUSH_INT};
    curr_inst.op1_int_val = val;
    vector_push(code_seg, curr_inst);
}

void generate_param_push_string(str_t* val) {
    inst_t curr_inst = {.inst_code = INST_PUSH_STRING};
    curr_inst.op1_str_val = val;
    vector_push(code_seg, curr_inst);
}

size_t generate_int_to_double(size_t source) {
    inst_t curr_inst = {.inst_code = INST_INT_TO_DOUBLE};
    curr_inst.op1_addr = source;
    size_t new_addr = generate_push();
    curr_inst.res_addr = new_addr;
    vector_push(code_seg, curr_inst);
    return new_addr;
}

size_t generate_double_to_int(size_t source) {
    inst_t curr_inst = {.inst_code = INST_DOUBLE_TO_INT};
    curr_inst.op1_addr = source;
    size_t new_addr = generate_push();
    curr_inst.res_addr = new_addr;
    vector_push(code_seg, curr_inst);
    return new_addr;
}

void generate_mov(size_t dest, size_t source) {
    inst_t curr_inst = {.inst_code = INST_MOV};
    curr_inst.res_addr = dest;
    curr_inst.op1_addr = source;
    vector_push(code_seg, curr_inst);
}

void generate_mov_int(size_t dest, int val) {
    inst_t curr_inst = {.inst_code = INST_MOV_INT};
    curr_inst.res_addr = dest;
    curr_inst.op1_int_val = val;
    vector_push(code_seg, curr_inst);
}

void generate_mov_double(size_t dest, double val) {
    inst_t curr_inst = {.inst_code = INST_MOV_DOUBLE};
    curr_inst.res_addr = dest;
    curr_inst.op1_double_val = val;
    vector_push(code_seg, curr_inst);
}

void generate_mov_string(size_t dest, str_t* val) {
    inst_t curr_inst = {.inst_code = INST_MOV_STRING};
    curr_inst.res_addr = dest;
    curr_inst.op1_str_val = val;
    vector_push(code_seg, curr_inst);
}

void generate_cin_int(size_t dest) {
    inst_t curr_inst = {.inst_code = INST_CIN_INT};
    curr_inst.res_addr = dest;
    vector_push(code_seg, curr_inst);
}

void generate_cin_double(size_t dest) {
    inst_t curr_inst = {.inst_code = INST_CIN_DOUBLE};
    curr_inst.res_addr = dest;
    vector_push(code_seg, curr_inst);
}

void generate_cin_string(size_t dest){
    inst_t curr_inst = {.inst_code = INST_CIN_STRING};
    curr_inst.res_addr = dest;
    vector_push(code_seg, curr_inst);
}

void generate_cout_int(size_t source) {
    inst_t curr_inst = {.inst_code = INST_COUT_INT};
    curr_inst.op1_addr = source;
    vector_push(code_seg, curr_inst);
}

void generate_cout_double(size_t source) {
    inst_t curr_inst = {.inst_code = INST_COUT_DOUBLE};
    curr_inst.op1_addr = source;
    vector_push(code_seg, curr_inst);
}

void generate_cout_string(size_t source){
    inst_t curr_inst = {.inst_code = INST_COUT_STRING};
    curr_inst.op1_addr = source;
    vector_push(code_seg, curr_inst);
}

void generate_cout_int_lit(int val) {
    inst_t curr_inst = {.inst_code = INST_COUT_INT_LIT};
    curr_inst.op1_int_val = val;
    vector_push(code_seg, curr_inst);
}

void generate_cout_double_lit(double val) {
    inst_t curr_inst = {.inst_code = INST_COUT_DOUBLE_LIT};
    curr_inst.op1_double_val = val;
    vector_push(code_seg, curr_inst);
}

void generate_cout_string_lit(str_t* val){
    inst_t curr_inst = {.inst_code = INST_COUT_STRING_LIT};
    curr_inst.op1_str_val = val;
    vector_push(code_seg, curr_inst);
}

void generate_jump(size_t dest) {
    inst_t curr_inst = {.inst_code = INST_JUMP};
    curr_inst.res_addr = dest;
    vector_push(code_seg, curr_inst);
}

void generate_cond_jump(size_t dest, size_t source) {
    inst_t curr_inst = {.inst_code = INST_CONDITIONAL_JUMP};
    curr_inst.op1_addr = source;
    curr_inst.res_addr = dest;
    vector_push(code_seg, curr_inst);
}

void generate_neg_cond_jump(size_t dest, size_t source) {
    inst_t curr_inst = {.inst_code = INST_NEGATIVE_CONDITIONAL_JUMP};
    curr_inst.op1_addr = source;
    curr_inst.res_addr = dest;
    vector_push(code_seg, curr_inst);
}

void set_jump_addr(size_t inst_addr, size_t dest) {
    inst_t* inst = vector_at(code_seg, inst_addr);
    inst->res_addr = dest;
}

void generate_call(symbol_t* func, size_t param_cnt) {
    inst_t curr_inst = {.inst_code = INST_CALL};
    //store return address
    curr_inst.op1_addr = get_code_seg_top() + 1;
    //store count
    curr_inst.op2_addr = param_cnt;
    //set function address
    curr_inst.func = func;
    vector_push(code_seg, curr_inst);
}

void generate_return(size_t source, size_t param_cnt) {
    inst_t curr_inst = {.inst_code = INST_RET};
    //this value will be returned
    curr_inst.op1_addr = source;
    vector_push(code_seg, curr_inst);
    curr_inst.inst_code = INST_RESTORE;
    curr_inst.op1_addr = param_cnt;
    vector_push(code_seg, curr_inst);
}

void generate_return_double(double val, size_t param_cnt) {
    inst_t curr_inst = {.inst_code = INST_RET_DOUBLE};
    //this value will be returned
    curr_inst.op1_double_val = val;
    vector_push(code_seg, curr_inst);
    curr_inst.inst_code = INST_RESTORE;
    curr_inst.op1_addr = param_cnt;
    vector_push(code_seg, curr_inst);
}

void generate_return_int(int val, size_t param_cnt) {
    inst_t curr_inst = {.inst_code = INST_RET_INT};
    //this value will be returned
    curr_inst.op1_int_val = val;
    vector_push(code_seg, curr_inst);
    curr_inst.inst_code = INST_RESTORE;
    curr_inst.op1_addr = param_cnt;
    vector_push(code_seg, curr_inst);
}

void generate_return_string(str_t* val, size_t param_cnt) {
    inst_t curr_inst = {.inst_code = INST_RET_STRING};
    //this value will be returned
    curr_inst.op1_str_val = val;
    vector_push(code_seg, curr_inst);
    curr_inst.inst_code = INST_RESTORE;
    curr_inst.op1_addr = param_cnt;
    vector_push(code_seg, curr_inst);
}
void generate_no_return_exception() {
    inst_t curr_inst = {.inst_code = INST_NO_RET};
    vector_push(code_seg, curr_inst);

}

void generate_halt() {
    inst_t curr_inst = {.inst_code = INST_HALT};
    vector_push(code_seg, curr_inst);
}

//arithmetic operations
size_t generate_D_D(int oper,
expression_t* op_left, expression_t* op_right, \
bool op_l_convert, bool op_r_convert) {
    inst_t curr_inst;
    curr_inst.inst_code = oper + D_D_OFFSET;
    curr_inst.op1_addr = (op_l_convert) ? generate_int_to_double(op_left->addr) : op_left->addr;
    curr_inst.op2_addr = (op_r_convert) ? generate_int_to_double(op_right->addr) : op_right->addr;
    size_t new_addr = generate_push();
    curr_inst.res_addr = new_addr;
    vector_push(code_seg, curr_inst);
    return new_addr;
}

size_t generate_D_DL(int oper, \
expression_t* op_left, expression_t* op_right, \
bool op_l_convert, bool op_r_convert) {
    inst_t curr_inst;
    curr_inst.inst_code = oper + D_DL_OFFSET;
    curr_inst.op1_addr = (op_l_convert) ? generate_int_to_double(op_left->addr) : op_left->addr;
    curr_inst.op2_double_val = (op_r_convert) ? (double)op_right->int_val : op_right->double_val;
    size_t new_addr = generate_push();
    curr_inst.res_addr = new_addr;
    vector_push(code_seg, curr_inst);
    return new_addr;
}

size_t generate_DL_D(int oper, \
expression_t* op_left, expression_t* op_right, \
bool op_l_convert, bool op_r_convert) {
    inst_t curr_inst;
    curr_inst.inst_code = oper + DL_D_OFFSET;
    curr_inst.op1_double_val = (op_l_convert) ? (double)op_left->int_val : op_left->double_val;
    curr_inst.op2_addr = (op_r_convert) ? generate_int_to_double(op_right->addr) : op_right->addr;
    size_t new_addr = generate_push();
    curr_inst.res_addr = new_addr;
    vector_push(code_seg, curr_inst);
    return new_addr;
}

size_t generate_I_I(int oper, \
expression_t* op_left, expression_t* op_right) {
    inst_t curr_inst;
    curr_inst.inst_code = oper + I_I_OFFSET;
    curr_inst.op1_addr = op_left->addr;
    curr_inst.op2_addr = op_right->addr;
    size_t new_addr = generate_push();
    curr_inst.res_addr = new_addr;
    vector_push(code_seg, curr_inst);
    return new_addr;
}

size_t generate_I_IL(int oper, \
expression_t* op_left, expression_t* op_right) {
    inst_t curr_inst;
    curr_inst.inst_code = oper + I_IL_OFFSET;
    curr_inst.op1_addr = op_left->addr;
    curr_inst.op2_int_val = op_right->int_val;
    size_t new_addr = generate_push();
    curr_inst.res_addr = new_addr;
    vector_push(code_seg, curr_inst);
    return new_addr;
}

size_t generate_IL_I(int oper, \
expression_t* op_left, expression_t* op_right) {
    inst_t curr_inst;
    curr_inst.inst_code = oper + IL_I_OFFSET;
    curr_inst.op1_int_val = op_left->int_val;
    curr_inst.op2_addr = op_right->addr;
    size_t new_addr = generate_push();
    curr_inst.res_addr = new_addr;
    vector_push(code_seg, curr_inst);
    return new_addr;
}

size_t generate_S_S(int oper, \
expression_t* op_left, expression_t* op_right) {
    inst_t curr_inst;
    curr_inst.inst_code = oper + S_S_OFFSET;
    curr_inst.op1_addr = op_left->addr;
    curr_inst.op2_addr = op_right->addr;
    size_t new_addr = generate_push();
    curr_inst.res_addr = new_addr;
    vector_push(code_seg, curr_inst);
    return new_addr;
}

size_t generate_S_SL(int oper, \
expression_t* op_left, expression_t* op_right) {
    inst_t curr_inst;
    curr_inst.inst_code = oper + S_SL_OFFSET;
    curr_inst.op1_addr = op_left->addr;
    curr_inst.op2_str_val = op_right->str_val;
    size_t new_addr = generate_push();
    curr_inst.res_addr = new_addr;
    vector_push(code_seg, curr_inst);
    return new_addr;
}

size_t generate_SL_S(int oper, \
expression_t* op_left, expression_t* op_right) {
    inst_t curr_inst;
    curr_inst.inst_code = oper + SL_S_OFFSET;
    curr_inst.op1_str_val = op_left->str_val;
    curr_inst.op2_addr = op_right->addr;
    size_t new_addr = generate_push();
    curr_inst.res_addr = new_addr;
    vector_push(code_seg, curr_inst);
    return new_addr;
}

int solve_rel_DL_DL(op_relational_t oper, \
expression_t* op_left, expression_t* op_right, \
bool op_l_convert, bool op_r_convert) {
    double op1 = (op_l_convert) ? (double)op_left->int_val : op_left->double_val;
    double op2 = (op_r_convert) ? (double)op_right->int_val : op_right->double_val;
    return_rel(oper, op1, op2);
}

int solve_rel_IL_IL(op_relational_t oper, \
expression_t* op_left, expression_t* op_right) {
    return_rel(oper, op_left->int_val, op_right->int_val);
}

int solve_rel_SL_SL(op_relational_t oper, \
expression_t* op_left, expression_t* op_right) {
    switch (oper) {
        case OP_REL_LESS:
            return str_lt(op_left->str_val, op_right->str_val);
        case OP_REL_GREATER:
            return str_gt(op_left->str_val, op_right->str_val);
        case OP_REL_LESS_OR_EQ:
            return str_lt_eq(op_left->str_val, op_right->str_val);
        case OP_REL_GREATER_OR_EQ:
            return str_gt_eq(op_left->str_val, op_right->str_val);
        case OP_REL_EQUAL:
            return str_eq(op_left->str_val, op_right->str_val);
        case OP_REL_NOT_EQUAL:
            return str_neq(op_left->str_val, op_right->str_val);
        default:
            return -1;
    }
}

double solve_arith_DL_DL(op_arithmetic_t oper, \
expression_t* op_left, expression_t* op_right, \
bool op_l_convert, bool op_r_convert) {
    double op1 = (op_l_convert) ? (double)op_left->int_val : op_left->double_val;
    double op2 = (op_r_convert) ? (double)op_right->int_val : op_right->double_val;
    if (oper == OP_ARITH_DIVIDE && op2 == 0)
        error("Division by zero", ERROR_DIVZERO);
    return_arith(oper, op1, op2);
}

int solve_arith_IL_IL(op_arithmetic_t oper, \
expression_t* op_left, expression_t* op_right) {
    if (oper == OP_ARITH_DIVIDE && op_right->int_val == 0)
        error("Division by zero", ERROR_DIVZERO);
    return_arith(oper, op_left->int_val, op_right->int_val);
}

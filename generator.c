#include "generator.h"

size_t curr_stack_frame;
vector_inst_t* code_seg;

void generator_init() {
    code_seg = vector_init(VI_CODESEG);
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

void generate_halt() {
    inst_t curr_inst = {.inst_code = INST_HALT};
    vector_push(code_seg, curr_inst);
}

size_t get_code_seg_top() {
    return code_seg->size;
}

void init_new_stack_frame(size_t param_count) {
    curr_stack_frame = 1; //index 0 is reseved for return address
}

size_t generate_push() {
    inst_t curr_inst = {.inst_code = INST_PUSH};
    vector_push(code_seg, curr_inst);
    return curr_stack_frame++;
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

void generate_call() {

}

size_t generate_int_to_double(size_t int_addr) {
    inst_t curr_inst = {.inst_code = INST_INT_TO_DOUBLE};
    curr_inst.op1_addr = int_addr;
    //curr_inst.op2_addr = 0;
    size_t new_addr = generate_push();
    curr_inst.res_addr = new_addr;
    vector_push(code_seg, curr_inst);
    return new_addr;
}

size_t generate_double_to_int(size_t double_addr) {
    inst_t curr_inst = {.inst_code = INST_DOUBLE_TO_INT};
    curr_inst.op1_addr = double_addr;
    //curr_inst.op2_addr = 0;
    size_t new_addr = generate_push();
    curr_inst.res_addr = new_addr;
    vector_push(code_seg, curr_inst);
    return new_addr;
}

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
            //return op_left->str_val < op_right->str_val;
            return 0;
        case OP_REL_GREATER:
            //return op_left->str_val > op_right->str_val;
            return 0;
        case OP_REL_LESS_OR_EQ:
            //return op_left->str_val <= op_right->str_val;
            return 0;
        case OP_REL_GREATER_OR_EQ:
            //return op_left->str_val >= op_right->str_val;
            return 0;
        case OP_REL_EQUAL:
            //return op_left->str_val == op_right->str_val;
            return 0;
        case OP_REL_NOT_EQUAL:
            //return op_left->str_val != op_right->str_val;
            return 0;
        default:
            return -1;
    }
}

double solve_arith_DL_DL(op_arithmetic_t oper, \
expression_t* op_left, expression_t* op_right, \
bool op_l_convert, bool op_r_convert) {
    double op1 = (op_l_convert) ? (double)op_left->int_val : op_left->double_val;
    double op2 = (op_r_convert) ? (double)op_right->int_val : op_right->double_val;
    return_arith(oper, op1, op2);
}

int solve_arith_IL_IL(op_arithmetic_t oper, \
expression_t* op_left, expression_t* op_right) {
    return_arith(oper, op_left->int_val, op_right->int_val);
}

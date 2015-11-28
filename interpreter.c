#include "interpreter.h"

vector_data_seg_t* data_seg;
size_t inst_ptr;
size_t curr_stack_frame_ptr;
char string_buffer[256];

void interpreter_init() {
    data_seg = vector_init(VI_DATASEG);
    str_t* main_name = str_init();
    str_copy(main_name, "main");
    symbol_t* main_func = func_table_find(main_name);
    if (main_func == NULL || main_func->paramList != NULL || main_func->type != INT_DT) {
        error("Main function not found", ERROR_SEM);
    }
    inst_ptr = main_func->addr;
    curr_stack_frame_ptr = 0;
    data_seg->size = 1;
}

void run_program() {
    inst_t* inst;
    data_seg_t* op_left;
    data_seg_t* op_right;
    data_seg_t* res_addr;
    data_seg_t empty_item = {.ret_addr = 0};

    while (1) {
        inst = vector_at(code_seg, inst_ptr);
        switch(inst->inst_code) {
            case INST_MUL_D_D:
                OPER_ADDR_ADDR(*, double_val, double_val);
                break;
            case INST_DIV_D_D:
                //TODO: division by zero
                OPER_ADDR_ADDR(/, double_val, double_val);
                break;
            case INST_ADD_D_D:
                OPER_ADDR_ADDR(+, double_val, double_val);
                break;
            case INST_SUB_D_D:
                OPER_ADDR_ADDR(-, double_val, double_val);
                break;
            case INST_LESS_D_D:
                OPER_ADDR_ADDR(<, double_val, int_val);
                break;
            case INST_GREATER_D_D:
                OPER_ADDR_ADDR(>, double_val, int_val);
                break;
            case INST_LESS_OR_EQ_D_D:
                OPER_ADDR_ADDR(<=, double_val, int_val);
                break;
            case INST_GREATER_OR_EQ_D_D:
                OPER_ADDR_ADDR(>=, double_val, int_val);
                break;
            case INST_EQUAL_D_D:
                OPER_ADDR_ADDR(==, double_val, int_val);
                break;
            case INST_NOT_EQUAL_D_D:
                OPER_ADDR_ADDR(!=, double_val, int_val);
                break;

            case INST_MUL_D_DL:
                OPER_ADDR_LIT(*, double_val, double_val);
                break;
            case INST_DIV_D_DL:
                OPER_ADDR_LIT(/, double_val, double_val);
                break;
            case INST_ADD_D_DL:
                OPER_ADDR_LIT(+, double_val, double_val);
                break;
            case INST_SUB_D_DL:
                OPER_ADDR_LIT(-, double_val, double_val);
                break;
            case INST_LESS_D_DL:
                OPER_ADDR_LIT(<, double_val, int_val);
                break;
            case INST_GREATER_D_DL:
                OPER_ADDR_LIT(>, double_val, int_val);
                break;
            case INST_LESS_OR_EQ_D_DL:
                OPER_ADDR_LIT(<=, double_val, int_val);
                break;
            case INST_GREATER_OR_EQ_D_DL:
                OPER_ADDR_LIT(>=, double_val, int_val);
                break;
            case INST_EQUAL_D_DL:
                OPER_ADDR_LIT(==, double_val, int_val);
                break;
            case INST_NOT_EQUAL_D_DL:
                OPER_ADDR_LIT(!=, double_val, int_val);
                break;

            case INST_MUL_DL_D:
                OPER_LIT_ADDR(*, double_val, double_val);
                break;
            case INST_DIV_DL_D:
                OPER_LIT_ADDR(/, double_val, double_val);
                break;
            case INST_ADD_DL_D:
                OPER_LIT_ADDR(+, double_val, double_val);
                break;
            case INST_SUB_DL_D:
                OPER_LIT_ADDR(-, double_val, double_val);
                break;
            case INST_LESS_DL_D:
                OPER_LIT_ADDR(<, double_val, int_val);
                break;
            case INST_GREATER_DL_D:
                OPER_LIT_ADDR(>, double_val, int_val);
                break;
            case INST_LESS_OR_EQ_DL_D:
                OPER_LIT_ADDR(<=, double_val, int_val);
                break;
            case INST_GREATER_OR_EQ_DL_D:
                OPER_LIT_ADDR(>=, double_val, int_val);
                break;
            case INST_EQUAL_DL_D:
                OPER_LIT_ADDR(==, double_val, int_val);
                break;
            case INST_NOT_EQUAL_DL_D:
                OPER_LIT_ADDR(!=, double_val, int_val);
                break;
            // INT
            case INST_MUL_I_I:
                OPER_ADDR_ADDR(*, int_val, int_val);
                break;
            case INST_DIV_I_I:
                OPER_ADDR_ADDR(/, int_val, int_val);
                break;
            case INST_ADD_I_I:
                OPER_ADDR_ADDR(+, int_val, int_val);
                break;
            case INST_SUB_I_I:
                OPER_ADDR_ADDR(-, int_val, int_val);
                break;
            case INST_LESS_I_I:
                OPER_ADDR_ADDR(<, int_val, int_val);
                break;
            case INST_GREATER_I_I:
                OPER_ADDR_ADDR(>, int_val, int_val);
                break;
            case INST_LESS_OR_EQ_I_I:
                OPER_ADDR_ADDR(<=, int_val, int_val);
                break;
            case INST_GREATER_OR_EQ_I_I:
                OPER_ADDR_ADDR(>=, int_val, int_val);
                break;
            case INST_EQUAL_I_I:
                OPER_ADDR_ADDR(==, int_val, int_val);
                break;
            case INST_NOT_EQUAL_I_I:
                OPER_ADDR_ADDR(!=, int_val, int_val);
                break;

            case INST_MUL_I_IL:
                OPER_ADDR_LIT(*, int_val, int_val);
                break;
            case INST_DIV_I_IL:
                OPER_ADDR_LIT(/, int_val, int_val);
                break;
            case INST_ADD_I_IL:
                OPER_ADDR_LIT(+, int_val, int_val);
                break;
            case INST_SUB_I_IL:
                OPER_ADDR_LIT(-, int_val, int_val);
                break;
            case INST_LESS_I_IL:
                OPER_ADDR_LIT(<, int_val, int_val);
                break;
            case INST_GREATER_I_IL:
                OPER_ADDR_LIT(>, int_val, int_val);
                break;
            case INST_LESS_OR_EQ_I_IL:
                OPER_ADDR_LIT(<=, int_val, int_val);
                break;
            case INST_GREATER_OR_EQ_I_IL:
                OPER_ADDR_LIT(>=, int_val, int_val);
                break;
            case INST_EQUAL_I_IL:
                OPER_ADDR_LIT(==, int_val, int_val);
                break;
            case INST_NOT_EQUAL_I_IL:
                OPER_ADDR_LIT(!=, int_val, int_val);
                break;

            case INST_MUL_IL_I:
                OPER_LIT_ADDR(*, int_val, int_val);
                break;
            case INST_DIV_IL_I:
                OPER_LIT_ADDR(/, int_val, int_val);
                break;
            case INST_ADD_IL_I:
                OPER_LIT_ADDR(+, int_val, int_val);
                break;
            case INST_SUB_IL_I:
                OPER_LIT_ADDR(-, int_val, int_val);
                break;
            case INST_LESS_IL_I:
                OPER_LIT_ADDR(<, int_val, int_val);
                break;
            case INST_GREATER_IL_I:
                OPER_LIT_ADDR(>, int_val, int_val);
                break;
            case INST_LESS_OR_EQ_IL_I:
                OPER_LIT_ADDR(<=, int_val, int_val);
                break;
            case INST_GREATER_OR_EQ_IL_I:
                OPER_LIT_ADDR(>=, int_val, int_val);
                break;
            case INST_EQUAL_IL_I:
                OPER_LIT_ADDR(==, int_val, int_val);
                break;
            case INST_NOT_EQUAL_IL_I:
                OPER_LIT_ADDR(!=, int_val, int_val);
                break;
            // STRING
            case INST_LESS_S_S:
                EXECSTR_ADDR_ADDR(less);
                break;
            case INST_GREATER_S_S:
                EXECSTR_ADDR_ADDR(greater);
                break;
            case INST_LESS_OR_EQ_S_S:
                EXECSTR_ADDR_ADDR(lesseq);
                break;
            case INST_GREATER_OR_EQ_S_S:
                EXECSTR_ADDR_ADDR(greatereq);
                break;
            case INST_EQUAL_S_S:
                EXECSTR_ADDR_ADDR(equal);
                break;
            case INST_NOT_EQUAL_S_S:
                EXECSTR_ADDR_ADDR(noteq);
                break;

            case INST_LESS_S_SL:
                EXECSTR_ADDR_LIT(less);
                break;
            case INST_GREATER_S_SL:
                EXECSTR_ADDR_LIT(greater);
                break;
            case INST_LESS_OR_EQ_S_SL:
                EXECSTR_ADDR_LIT(lesseq);
                break;
            case INST_GREATER_OR_EQ_S_SL:
                EXECSTR_ADDR_LIT(greatereq);
                break;
            case INST_EQUAL_S_SL:
                EXECSTR_ADDR_LIT(equal);
                break;
            case INST_NOT_EQUAL_S_SL:
                EXECSTR_ADDR_LIT(noteq);
                break;

            case INST_LESS_SL_S:
                EXECSTR_LIT_ADDR(less);
                break;
            case INST_GREATER_SL_S:
                EXECSTR_LIT_ADDR(greater);
                break;
            case INST_LESS_OR_EQ_SL_S:
                EXECSTR_LIT_ADDR(lesseq);
                break;
            case INST_GREATER_OR_EQ_SL_S:
                EXECSTR_LIT_ADDR(greatereq);
                break;
            case INST_EQUAL_SL_S:
                EXECSTR_LIT_ADDR(equal);
                break;
            case INST_NOT_EQUAL_SL_S:
                EXECSTR_LIT_ADDR(noteq);
                break;
            case INST_MOV:
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                *res_addr = *op_left;
                break;
            case INST_MOV_INT:
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                res_addr->int_val = inst->op1_int_val;
                break;
            case INST_MOV_DOUBLE:
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                res_addr->double_val = inst->op1_double_val;
                break;
            case INST_MOV_STRING:
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                res_addr->str_val = inst->op1_str_val;
                break;
            case INST_PUSH:
                vector_push(data_seg, empty_item);
                break;
            case INST_INT_TO_DOUBLE:
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                res_addr->double_val = (double)op_left->int_val;
                break;
            case INST_DOUBLE_TO_INT:
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                res_addr->int_val = (int)op_left->int_val;
                break;
            case INST_HALT:
                warning("Halt!");
                return;
                break;
            case INST_CIN_INT:
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                scanf("%d", &(res_addr->int_val));
                break;
            case INST_CIN_DOUBLE:
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                scanf("%lf", &(res_addr->double_val));
                break;
            case INST_CIN_STRING:
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                str_t* new_string = str_init();
                do {
                    scanf("%s", string_buffer);
                    str_append(new_string, string_buffer);
                } while (isspace(string_buffer[254]));
                res_addr->str_val = new_string;
                break;
            case INST_COUT_INT:
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                printf("%d", op_left->int_val);
                break;
            case INST_COUT_DOUBLE:
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                printf("%lf", op_left->double_val);
                break;
            case INST_COUT_STRING:
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                printf("%s", op_left->str_val->c_str);
                break;
            case INST_COUT_INT_LIT:
                printf("%d", inst->op1_int_val);
                break;
            case INST_COUT_DOUBLE_LIT:
                printf("%lf", inst->op1_double_val);
                break;
            case INST_COUT_STRING_LIT:
                printf("%s", inst->op1_str_val->c_str);
                break;
            default:
                ;
        } //end switch
        ++inst_ptr;
    } //end while
}

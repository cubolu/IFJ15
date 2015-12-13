#include "interpreter.h"

vector_data_seg_t* data_seg;
size_t inst_ptr;
size_t curr_stack_frame_ptr;

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
    data_seg->size = 4; //return address + data for INST_RESTORE
}

void run_program() {
    inst_t* inst;
    size_t size;
    data_seg_t* op_left;
    data_seg_t* op_right;
    data_seg_t* res_addr;
    data_seg_t* prev_size;
    data_seg_t* prev_stack_frame_ptr;
    data_seg_t* op_1;
    data_seg_t* op_2;
    data_seg_t* op_3;
    data_seg_t push_value;
    data_seg_t empty_value = {.inst_addr = 0};
    int scanf_ret_val;
    char string_buffer[256];

    while (1) {
        inst = vector_at(code_seg, inst_ptr);
        switch(inst->inst_code) {
            case INST_MUL_D_D:
                OPER_ADDR_ADDR(*, double_val, double_val);
                break;
            case INST_DIV_D_D:
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                op_right = vector_at(data_seg, inst->op2_addr + curr_stack_frame_ptr);
                if (op_right->double_val != 0)
                    res_addr->double_val = op_left->double_val / op_right->double_val;
                else
                    error("Division by zero", ERROR_DIVZERO);
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
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                if (inst->op2_double_val != 0)
                    res_addr->double_val = op_left->double_val / inst->op2_double_val;
                else
                    error("Division by zero", ERROR_DIVZERO);
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
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                op_right = vector_at(data_seg, inst->op2_addr + curr_stack_frame_ptr);
                if (op_right->double_val != 0)
                    res_addr->double_val = inst->op1_double_val / op_right->double_val;
                else
                    error("Division by zero", ERROR_DIVZERO);
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
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                op_right = vector_at(data_seg, inst->op2_addr + curr_stack_frame_ptr);
                if (op_right->int_val != 0)
                    res_addr->int_val = op_left->int_val / op_right->int_val;
                else
                    error("Division by zero", ERROR_DIVZERO);
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
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                if (inst->op2_int_val != 0)
                    res_addr->int_val = op_left->int_val / inst->op2_int_val;
                else
                    error("Division by zero", ERROR_DIVZERO);
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
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                op_right = vector_at(data_seg, inst->op2_addr + curr_stack_frame_ptr);
                if (op_right->int_val != 0)
                    res_addr->int_val = inst->op1_int_val / op_right->int_val;
                else
                    error("Division by zero", ERROR_DIVZERO);
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
                EXECSTR_ADDR_ADDR(str_lt);
                break;
            case INST_GREATER_S_S:
                EXECSTR_ADDR_ADDR(str_gt);
                break;
            case INST_LESS_OR_EQ_S_S:
                EXECSTR_ADDR_ADDR(str_lt_eq);
                break;
            case INST_GREATER_OR_EQ_S_S:
                EXECSTR_ADDR_ADDR(str_gt_eq);
                break;
            case INST_EQUAL_S_S:
                EXECSTR_ADDR_ADDR(str_eq);
                break;
            case INST_NOT_EQUAL_S_S:
                EXECSTR_ADDR_ADDR(str_neq);
                break;

            case INST_LESS_S_SL:
                EXECSTR_ADDR_LIT(str_lt);
                break;
            case INST_GREATER_S_SL:
                EXECSTR_ADDR_LIT(str_gt);
                break;
            case INST_LESS_OR_EQ_S_SL:
                EXECSTR_ADDR_LIT(str_lt_eq);
                break;
            case INST_GREATER_OR_EQ_S_SL:
                EXECSTR_ADDR_LIT(str_gt_eq);
                break;
            case INST_EQUAL_S_SL:
                EXECSTR_ADDR_LIT(str_eq);
                break;
            case INST_NOT_EQUAL_S_SL:
                EXECSTR_ADDR_LIT(str_neq);
                break;

            case INST_LESS_SL_S:
                EXECSTR_LIT_ADDR(str_lt);
                break;
            case INST_GREATER_SL_S:
                EXECSTR_LIT_ADDR(str_gt);
                break;
            case INST_LESS_OR_EQ_SL_S:
                EXECSTR_LIT_ADDR(str_lt_eq);
                break;
            case INST_GREATER_OR_EQ_SL_S:
                EXECSTR_LIT_ADDR(str_gt_eq);
                break;
            case INST_EQUAL_SL_S:
                EXECSTR_LIT_ADDR(str_eq);
                break;
            case INST_NOT_EQUAL_SL_S:
                EXECSTR_LIT_ADDR(str_neq);
                break;

            case INST_PUSH:
                vector_push(data_seg, empty_value);
                break;
            case INST_PUSH_VALUE:
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                push_value = *op_left;
                vector_push(data_seg, push_value);
                break;
            case INST_CONV_TO_DOUBLE_AND_PUSH_VALUE:
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                push_value.double_val = (double)op_left->int_val;
                vector_push(data_seg, push_value);
                break;
            case INST_CONV_TO_INT_AND_PUSH_VALUE:
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                push_value.int_val = (int)op_left->double_val;
                vector_push(data_seg, push_value);
                break;
            case INST_PUSH_DOUBLE:
                push_value.double_val = inst->op1_double_val;
                vector_push(data_seg, push_value);
                break;
            case INST_PUSH_INT:
                push_value.int_val = inst->op1_int_val;
                vector_push(data_seg, push_value);
                break;
            case INST_PUSH_STRING:
                push_value.str_val = inst->op1_str_val;
                vector_push(data_seg, push_value);
                break;

            case INST_INT_TO_DOUBLE:
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                res_addr->double_val = (double)op_left->int_val;
                break;
            case INST_DOUBLE_TO_INT:
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                res_addr->int_val = (int)op_left->double_val;
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

            case INST_CIN_INT:
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                scanf_ret_val = scanf("%d", &(res_addr->int_val));
                if (scanf_ret_val == 0)
                    error("Input value is not an integer", ERROR_NUM_INPUT);
                else if (scanf_ret_val == EOF)
                    error("Read failure occurs", ERROR_NUM_INPUT);
                break;
            case INST_CIN_DOUBLE:
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                scanf_ret_val = scanf("%lf", &(res_addr->double_val));
                if (scanf_ret_val == 0)
                    error("Input value is not a floating-point number", ERROR_NUM_INPUT);
                else if (scanf_ret_val == EOF)
                    error("Read failure occurs", ERROR_NUM_INPUT);
                break;
            case INST_CIN_STRING:
                res_addr = vector_at(data_seg, inst->res_addr + curr_stack_frame_ptr);
                str_t* new_string = str_init();
                string_buffer[0] = '\0';
                do {
                    string_buffer[254] = 0;
                    scanf("%255s", string_buffer);
                    str_append(new_string, string_buffer);
                } while (string_buffer[254]!=0);
                res_addr->str_val = new_string;
                break;

            case INST_COUT_INT:
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                printf("%d", op_left->int_val);
                break;
            case INST_COUT_DOUBLE:
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                printf("%g", op_left->double_val);
                break;
            case INST_COUT_STRING:
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                printf("%s", op_left->str_val->c_str);
                break;
            case INST_COUT_INT_LIT:
                printf("%d", inst->op1_int_val);
                break;
            case INST_COUT_DOUBLE_LIT:
                printf("%g", inst->op1_double_val);
                break;
            case INST_COUT_STRING_LIT:
                printf("%s", inst->op1_str_val->c_str);
                break;

            case INST_JUMP:
                inst_ptr = inst->res_addr - 1; //-1 because of ++inst_ptr at the end
                break;
            case INST_CONDITIONAL_JUMP:
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                if (op_left->int_val)
                    inst_ptr = inst->res_addr - 1;
                break;
            case INST_NEGATIVE_CONDITIONAL_JUMP:
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                if (!(op_left->int_val))
                    inst_ptr = inst->res_addr - 1;
                break;

            case INST_CALL:
                //push current data segment size without parameters
                size = data_seg->size - inst->op2_addr;
                push_value.inst_addr = size;
                vector_push(data_seg, push_value);
                //push current stack frame pointer
                push_value.inst_addr = curr_stack_frame_ptr;
                vector_push(data_seg, push_value);
                //push return address
                push_value.inst_addr = inst->op1_addr;
                vector_push(data_seg, push_value);
                //call function at address inst->res_addr
                inst_ptr = inst->res_addr - 1; //-1 because of ++inst_ptr at the end
                //change stack frame
                curr_stack_frame_ptr = size - 1;
                break;
            case INST_RET:
                //move return value to its address
                res_addr = vector_at(data_seg, curr_stack_frame_ptr);
                op_left = vector_at(data_seg, inst->op1_addr + curr_stack_frame_ptr);
                *res_addr = *op_left;
                break;
            case INST_RET_DOUBLE:
                //move return value to its address
                res_addr = vector_at(data_seg, curr_stack_frame_ptr);
                res_addr->double_val = inst->op1_double_val;
                break;
            case INST_RET_INT:
                //move return value to its address
                res_addr = vector_at(data_seg, curr_stack_frame_ptr);
                res_addr->int_val = inst->op1_int_val;
                break;
            case INST_RET_STRING:
                //move return value to its address
                res_addr = vector_at(data_seg, curr_stack_frame_ptr);
                res_addr->str_val = inst->op1_str_val;
                break;
            case INST_RESTORE:
                //restore previous stack frame
                size = inst->op1_addr + 1; //parameters count + res_addr
                prev_size = vector_at(data_seg, curr_stack_frame_ptr + size);
                prev_stack_frame_ptr = vector_at(data_seg, curr_stack_frame_ptr + size + 1);
                res_addr = vector_at(data_seg, curr_stack_frame_ptr + size + 2);
                data_seg->size = prev_size->inst_addr;
                curr_stack_frame_ptr = prev_stack_frame_ptr->inst_addr;
                inst_ptr = res_addr->inst_addr - 1; //-1 because of ++inst_ptr at the end
                break;
            case INST_NO_RET:
                error("Function call with no return", ERROR_UNDEF);

            case INST_BUILTIN_LENGTH:
                op_1 = vector_at(data_seg, curr_stack_frame_ptr + 1);
                res_addr = vector_at(data_seg, curr_stack_frame_ptr);
                res_addr->int_val = length(op_1->str_val);
                break;
            case INST_BUILTIN_SUBSTR:
                op_1 = vector_at(data_seg, curr_stack_frame_ptr + 1);
                op_2 = vector_at(data_seg, curr_stack_frame_ptr + 2);
                op_3 = vector_at(data_seg, curr_stack_frame_ptr + 3);
                res_addr = vector_at(data_seg, curr_stack_frame_ptr);
                res_addr->str_val = substr(op_1->str_val, op_2->int_val, op_3->int_val);
                break;
            case INST_BUILTIN_CONCAT:
                op_1 = vector_at(data_seg, curr_stack_frame_ptr + 1);
                op_2 = vector_at(data_seg, curr_stack_frame_ptr + 2);
                res_addr = vector_at(data_seg, curr_stack_frame_ptr);
                res_addr->str_val = concat(op_1->str_val, op_2->str_val);
                break;
            case INST_BUILTIN_FIND:
                op_1 = vector_at(data_seg, curr_stack_frame_ptr + 1);
                op_2 = vector_at(data_seg, curr_stack_frame_ptr + 2);
                res_addr = vector_at(data_seg, curr_stack_frame_ptr);
                res_addr->int_val = find(op_1->str_val, op_2->str_val);
                break;
            case INST_BUILTIN_SORT:
                op_1 = vector_at(data_seg, curr_stack_frame_ptr + 1);
                res_addr = vector_at(data_seg, curr_stack_frame_ptr);
                res_addr->str_val = sort(op_1->str_val);
                break;

            case INST_DATA_SEGMENT_RESTORE:
                data_seg->size = inst->op1_addr + curr_stack_frame_ptr;
                break;
            case INST_HALT:
                return;

            default:
                warning("Unsupported instruction code");
        } //end switch
        ++inst_ptr;
    } //end while
}

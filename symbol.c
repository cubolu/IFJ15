#include "symbol.h"

bool is_equal_func(symbol_t* func1, symbol_t* func2);
bool is_equal_paramlist(ulist_str_t* paramList1, ulist_str_t* paramList2);
void load_builtin_functions();

void symbol_table_init() {
    var_table_vector = vector_init(VI_HTABLE);
    func_symtable = htable_init();
    load_builtin_functions();
}

symbol_t* var_table_find(str_t* name) {
    return vector_find(var_table_vector, name);
}

//symbol_t* var_table_curr_find(str_t* name) {
//    return htable_get(curr_table, name);
//}

symbol_t* func_table_find(str_t* name) {
    return htable_get(func_symtable, name);
}

void var_table_add(symbol_t* var) {
    symbol_t* prevEntry = htable_get(curr_symtable, var->name);
    if (prevEntry != NULL)
        error("Variable redefinition", ERROR_SEM);
    else
        htable_set(curr_symtable, var->name, *var);
}

void func_table_add(symbol_t* func) {
    symbol_t* prevEntry = htable_get(func_symtable, func->name);
    if (prevEntry != NULL && is_equal_func(prevEntry, func) == false) {
        error("IFJ15 doesn't support function overloading.", ERROR_SEM);
    } else if (prevEntry == NULL)
        htable_set(func_symtable, func->name, *func);
}

void var_table_scope_enter() {
    curr_symtable = htable_init();
    vector_push(var_table_vector, curr_symtable);
}

void var_table_scope_exit() {
    htable_t* left_htable = vector_pop(var_table_vector);
    curr_symtable = vector_top(var_table_vector);
    ifj15_free(left_htable);
}

bool is_equal_paramlist(ulist_str_t* paramList1, ulist_str_t* paramList2) {
    unode_str_t* iter1 = paramList1->front;
    unode_str_t* iter2 = paramList2->front;
    while (iter1 != NULL && iter2 != NULL) {
        if((iter1->item.type == STRING_DT || iter2->item.type == STRING_DT) &&
            iter1->item.type != iter2->item.type)
            return false;
        iter1 = iter1->next;
        iter2 = iter2->next;
    }
    if (iter1 != NULL || iter2 != NULL)
        return false; //not the same amount of parameters
    return true;
}

bool is_equal_paramtypes(vector_int_t* paramTypes, ulist_str_t* paramList) {
    int iter1 = paramTypes->size -1;
    unode_str_t* iter2 = paramList->front;
    e_data_t iter1_type;
    while (iter1 >= 0 && iter2 != NULL) {
        iter1_type = vector_at(paramTypes, iter1);
        if((iter1_type == STRING_DT || iter2->item.type == STRING_DT) &&
            iter1_type != iter2->item.type)
            return false;
        --iter1;
        iter2 = iter2->next;
    }
    if (iter1 >= 0 || iter2 != NULL)
        return false; //not the same amout of parameters
    return true;
}

bool is_equal_func(symbol_t* func1, symbol_t* func2) {
    //compare names, number and type of arguments
    if (!str_equal(func1->name, func2->name)
        || (func1->paramList == NULL && func2->paramList != NULL)
        || (func1->paramList != NULL && func2->paramList == NULL)) {
        return false;
    }
    if (func1->paramList != NULL) //means func2->paramList is not NULL
        return is_equal_paramlist(func1->paramList, func2->paramList);
    return true;
}

bool is_valid_func_call(func_call_t* funcCall, symbol_t* funcSym) {
    if (!str_equal(funcCall->name, funcSym->name)
        || (funcCall->paramTypes == NULL && funcSym->paramList != NULL)
        || (funcCall->paramTypes != NULL && funcSym->paramList == NULL)) {
        return false;
    }
    if (funcCall->paramTypes != NULL)
        return is_equal_paramtypes(funcCall->paramTypes, funcSym->paramList);
    return true;
}

void var_init() {
    bufferVar.name = NULL;
    bufferVar.sym = FUN_SYM;
    bufferVar.type = -1;
    bufferVar.def = false;
    bufferVar.addr = 0;
    bufferVar.paramList = NULL;
}

void var_set_type(e_data_t type) {
    bufferVar.type = type;
}

void var_set_addr(size_t addr) {
    bufferVar.addr = addr;
}

void var_set_name(str_t* name) {
    bufferVar.name = name;
}

void var_set_initialized() {
    bufferVar.def = true;
}

e_data_t var_get_type() {
    return bufferVar.type;
}

symbol_t* var_finish() {
    return &bufferVar;
}

void func_init() {
    bufferFunc.name = NULL;
    bufferFunc.sym = FUN_SYM;
    bufferFunc.type = -1;
    bufferFunc.def = false;
    bufferFunc.addr = 0;
    bufferFunc.paramList = NULL;
    bufferFuncParamCnt = 0;
}

void func_set_name(str_t* name) {
    bufferFunc.name = name;
}

void func_set_start_addr(size_t addr) {
    bufferFunc.addr = addr;
}

void func_add_param(symbol_t* paramSymbol) {
    if (bufferFunc.paramList == NULL) {
        bufferFunc.paramList = ulist_str_init();
    }
    if (!ulist_get(bufferFunc.paramList, paramSymbol->name))
        ulist_set(bufferFunc.paramList, paramSymbol->name, *paramSymbol);
    else
        error("Two function parameters with the same name", ERROR_SEM);
}

void func_set_defined() {
    bufferFunc.def = true;
}

void func_set_return_type(e_data_t retType) {
    bufferFunc.type = retType;
}

void func_set_param_count(size_t cnt) {
    bufferFuncParamCnt = cnt;
}

str_t* func_get_name() {
    return bufferFunc.name;
}

e_data_t func_get_return_type() {
    return bufferFunc.type;
}

size_t func_get_param_count() {
    return bufferFuncParamCnt;
}

symbol_t* func_finish() {
    return &bufferFunc;
}

void func_call_init() {
    bufferFuncCall.name = NULL;
    bufferFuncCall.paramTypes = NULL;
}
void func_call_set_name(str_t* name) {
    bufferFuncCall.name = name;
}
void func_call_add_param(e_data_t type){
    if (bufferFuncCall.paramTypes == NULL) {
        bufferFuncCall.paramTypes = vector_init(VI_INT, 1);
    }
    vector_push(bufferFuncCall.paramTypes, (int) type);
}

func_call_t* func_call_finish() {
    return &bufferFuncCall;
}

void load_builtin_functions() {
    str_t* lengthName = str_init();
    str_t* substrName = str_init();
    str_t* concatName = str_init();
    str_t* findName = str_init();
    str_t* sortName = str_init();
    str_copy(lengthName, "length");
    str_copy(substrName, "substr");
    str_copy(concatName, "concat");
    str_copy(findName, "find");
    str_copy(sortName, "sort");
    str_t* paramName1 = str_init();
    str_t* paramName2 = str_init();
    str_t* paramName3 = str_init();
    str_copy(paramName1, "p1");
    str_copy(paramName2, "p2");
    str_copy(paramName3, "p3");
    var_init();

    func_init();
    func_set_return_type(INT_DT);
    func_set_name(lengthName);
    func_set_start_addr(0);
    var_set_type(STRING_DT);
    var_set_name(paramName1);
    func_add_param(var_finish());
    func_set_defined();
    func_table_add(func_finish());

    func_init();
    func_set_return_type(STRING_DT);
    func_set_name(substrName);
    func_set_start_addr(2);
    func_add_param(var_finish());
    var_set_type(INT_DT);
    var_set_name(paramName2);
    func_add_param(var_finish());
    var_set_name(paramName3);
    func_add_param(var_finish());
    func_set_defined();
    func_table_add(func_finish());

    func_init();
    func_set_return_type(STRING_DT);
    func_set_name(concatName);
    func_set_start_addr(4);
    var_set_type(STRING_DT);
    var_set_name(paramName1);
    func_add_param(var_finish());
    var_set_name(paramName2);
    func_add_param(var_finish());
    func_set_defined();
    func_table_add(func_finish());

    func_init();
    func_set_return_type(INT_DT);
    func_set_name(findName);
    func_set_start_addr(6);
    var_set_name(paramName1);
    func_add_param(var_finish());
    var_set_name(paramName2);
    func_add_param(var_finish());
    func_set_defined();
    func_table_add(func_finish());

    func_init();
    func_set_return_type(STRING_DT);
    func_set_name(sortName);
    func_set_start_addr(8);
    var_set_name(paramName1);
    func_add_param(var_finish());
    func_set_defined();
    func_table_add(func_finish());
}

void check_rule_rel(op_relational_t op, vector_expr_t* expr_buffer) {
    //pop operands from stack, rigth is on top
    expression_t right_expr = vector_pop(expr_buffer);
    expression_t left_expr = vector_pop(expr_buffer);
    expression_t ret_expr = {.type = INT_DT};
    //TODO: use look up table, inspired by Precedence table
    switch (left_expr.type) {
        case DOUBLE_DT:
            switch (right_expr.type) {
                case DOUBLE_DT:
                    //DOUBLE_DT OP DOUBLE_DT
                    ret_expr.addr = generate_D_D(op, &left_expr, &right_expr, false, false);
                    break;
                case DOUBLE_LIT_DT:
                    //DOUBLE_DT OP DOUBLE_LIT_DT
                    ret_expr.addr = generate_D_DL(op, &left_expr, &right_expr, false, false);
                    break;
                case INT_DT:
                    //DOUBLE_DT OP INT_DT
                    ret_expr.addr = generate_D_D(op, &left_expr, &right_expr, false, true);
                    break;
                case INT_LIT_DT:
                    //DOUBLE_DT OP INT_LIT_DT
                    ret_expr.addr = generate_D_DL(op, &left_expr, &right_expr, false, true);
                    break;
                default:
                    error("Incompatible operators types", ERROR_TYPE_COMPAT);
            }
            break;
        case DOUBLE_LIT_DT:
            switch (right_expr.type) {
                case DOUBLE_DT:
                    //DOUBLE_LIT_DT OP DOUBLE_DT
                    ret_expr.addr = generate_DL_D(op, &left_expr, &right_expr, false, false);
                    break;
                case DOUBLE_LIT_DT:
                    //DOUBLE_LIT_DT OP DOUBLE_LIT_DT
                    ret_expr.type = INT_LIT_DT;
                    ret_expr.int_val = solve_rel_DL_DL(op, &left_expr, &right_expr, false, false);
                    break;
                case INT_DT:
                    //DOUBLE_LIT_DT OP INT_DT
                    ret_expr.addr = generate_DL_D(op, &left_expr, &right_expr, false, true);
                    break;
                case INT_LIT_DT:
                    //DOUBLE_LIT_DT OP INT_LIT_DT
                    ret_expr.type = INT_LIT_DT;
                    ret_expr.int_val = solve_rel_DL_DL(op, &left_expr, &right_expr, false, true);
                    break;
                default:
                    error("Incompatible operators types", ERROR_TYPE_COMPAT);
            }
            break;
        case INT_DT:
            switch (right_expr.type) {
                case DOUBLE_DT:
                    //INT_DT OP DOUBLE_DT
                    ret_expr.addr = generate_D_D(op, &left_expr, &right_expr, true, false);
                    break;
                case DOUBLE_LIT_DT:
                    //INT_DT OP DOUBLE_LIT_DT
                    ret_expr.addr = generate_D_DL(op, &left_expr, &right_expr, true, false);
                    break;
                case INT_DT:
                    //INT_DT OP INT_DT
                    ret_expr.addr = generate_I_I(op, &left_expr, &right_expr);
                    break;
                case INT_LIT_DT:
                    //INT_DT OP INT_LIT_DT
                    ret_expr.addr = generate_I_IL(op, &left_expr, &right_expr);
                    break;
                default:
                    error("Incompatible operators types", ERROR_TYPE_COMPAT);
            }
            break;
        case INT_LIT_DT:
            switch (right_expr.type) {
                case DOUBLE_DT:
                    //INT_LIT_DT OP DOUBLE_DT
                    ret_expr.addr = generate_DL_D(op, &left_expr, &right_expr, true, false);
                    break;
                case DOUBLE_LIT_DT:
                    //INT_LIT_DT OP DOUBLE_LIT_DT
                    ret_expr.type = INT_LIT_DT;
                    ret_expr.int_val = solve_rel_DL_DL(op, &left_expr, &right_expr, true, false);
                    break;
                case INT_DT:
                    //INT_LIT_DT OP INT_DT
                    ret_expr.addr = generate_IL_I(op, &left_expr, &right_expr);
                    break;
                case INT_LIT_DT:
                    //INT_LIT_DT OP INT_LIT_DT
                    ret_expr.type = INT_LIT_DT;
                    ret_expr.int_val = solve_rel_IL_IL(op, &left_expr, &right_expr);
                    break;
                default:
                    error("Incompatible operators types", ERROR_TYPE_COMPAT);
            }
            break;
        case STRING_DT:
            switch (right_expr.type) {
                case STRING_DT:
                    //STRING_DT OP STRING_DT
                    ret_expr.addr = generate_S_S(op, &left_expr, &right_expr);
                    break;
                case STRING_LIT_DT:
                    //STRING_DT OP STRING_LIT_DT
                    ret_expr.addr = generate_S_SL(op, &left_expr, &right_expr);
                    break;
                default:
                    error("Incompatible operators types", ERROR_TYPE_COMPAT);
            }
            break;
        case STRING_LIT_DT:
            switch (right_expr.type) {
                case STRING_DT:
                    //STRING_LIT_DT OP STRING_DT
                    ret_expr.addr = generate_SL_S(op, &left_expr, &right_expr);
                    break;
                case STRING_LIT_DT:
                    //STRING_DT OP STRING_DT
                    ret_expr.type = INT_LIT_DT;
                    ret_expr.int_val = solve_rel_SL_SL(op, &left_expr, &right_expr);
                    break;
                default:
                    error("Incompatible operators types", ERROR_TYPE_COMPAT);
            }
            break;
        default:
            error("Unknown operator type", ERROR_INTERNAL);
    }
    vector_push(expr_buffer, ret_expr);
}

void check_rule_arith(op_arithmetic_t op, vector_expr_t* expr_buffer) {
    expression_t right_expr = vector_pop(expr_buffer);
    expression_t left_expr = vector_pop(expr_buffer);
    expression_t ret_expr = {.type = NONE_DT};
    switch (left_expr.type) {
        case DOUBLE_DT:
            ret_expr.type = DOUBLE_DT;
            switch (right_expr.type) {
                case DOUBLE_DT:
                    //DOUBLE_DT OP DOUBLE_DT
                    ret_expr.addr = generate_D_D(op, &left_expr, &right_expr, false, false);
                    break;
                case DOUBLE_LIT_DT:
                    //DOUBLE_DT OP DOUBLE_LIT_DT
                    ret_expr.addr = generate_D_DL(op, &left_expr, &right_expr, false, false);
                    break;
                case INT_DT:
                    //DOUBLE_DT OP INT_DT
                    ret_expr.addr = generate_D_D(op, &left_expr, &right_expr, false, true);
                    break;
                case INT_LIT_DT:
                    //DOUBLE_DT OP INT_LIT_DT
                    ret_expr.addr = generate_D_DL(op, &left_expr, &right_expr, false, true);
                    break;
                default:
                    error("Incompatible operators types", ERROR_TYPE_COMPAT);
            }
            break;
        case DOUBLE_LIT_DT:
            ret_expr.type = DOUBLE_DT;
            switch (right_expr.type) {
                case DOUBLE_DT:
                    //DOUBLE_LIT_DT OP DOUBLE_DT
                    ret_expr.addr = generate_DL_D(op, &left_expr, &right_expr, false, false);
                    break;
                case DOUBLE_LIT_DT:
                    //DOUBLE_LIT_DT OP DOUBLE_LIT_DT
                    ret_expr.type = DOUBLE_LIT_DT;
                    ret_expr.double_val = solve_arith_DL_DL(op, &left_expr, &right_expr, false, false);
                    break;
                case INT_DT:
                    //DOUBLE_LIT_DT OP INT_DT
                    ret_expr.addr = generate_DL_D(op, &left_expr, &right_expr, false, true);
                    break;
                case INT_LIT_DT:
                    //DOUBLE_LIT_DT OP INT_LIT_DT
                    ret_expr.type = DOUBLE_LIT_DT;
                    ret_expr.double_val = solve_arith_DL_DL(op, &left_expr, &right_expr, false, true);
                    break;
                default:
                    error("Incompatible operators types", ERROR_TYPE_COMPAT);
            }
            break;
        case INT_DT:
            switch (right_expr.type) {
                case DOUBLE_DT:
                    //INT_DT OP DOUBLE_DT
                    ret_expr.type = DOUBLE_DT;
                    ret_expr.addr = generate_D_D(op, &left_expr, &right_expr, true, false);
                    break;
                case DOUBLE_LIT_DT:
                    //INT_DT OP DOUBLE_LIT_DT
                    ret_expr.type = DOUBLE_DT;
                    ret_expr.addr = generate_D_DL(op, &left_expr, &right_expr, true, false);
                    break;
                case INT_DT:
                    //INT_DT OP INT_DT
                    ret_expr.type = INT_DT;
                    ret_expr.addr = generate_I_I(op, &left_expr, &right_expr);
                    break;
                case INT_LIT_DT:
                    //INT_DT OP INT_LIT_DT
                    ret_expr.type = INT_DT;
                    ret_expr.addr = generate_I_IL(op, &left_expr, &right_expr);
                    break;
                default:
                    error("Incompatible operators types", ERROR_TYPE_COMPAT);
            }
            break;
        case INT_LIT_DT:
            switch (right_expr.type) {
                case DOUBLE_DT:
                    //INT_LIT_DT OP DOUBLE_DT
                    ret_expr.type = DOUBLE_DT;
                    ret_expr.addr = generate_DL_D(op, &left_expr, &right_expr, true, false);
                    break;
                case DOUBLE_LIT_DT:
                    //INT_LIT_DT OP DOUBLE_LIT_DT
                    ret_expr.type = DOUBLE_LIT_DT;
                    ret_expr.double_val = solve_arith_DL_DL(op, &left_expr, &right_expr, true, false);
                    break;
                case INT_DT:
                    //INT_LIT_DT OP INT_DT
                    ret_expr.type = INT_DT;
                    ret_expr.addr = generate_IL_I(op, &left_expr, &right_expr);
                    break;
                case INT_LIT_DT:
                    //INT_LIT_DT OP INT_LIT_DT
                    ret_expr.type = INT_LIT_DT;
                    ret_expr.int_val = solve_arith_IL_IL(op, &left_expr, &right_expr);
                    break;
                default:
                    error("Incompatible operators types", ERROR_TYPE_COMPAT);
            }
            break;
        default:
            error("Incompatible operators types", ERROR_TYPE_COMPAT);
    }
    vector_push(expr_buffer, ret_expr);
}

void check_rule_id(token_t* last_token, vector_expr_t* expr_buffer) {
    expression_t expr;
    if (last_token->type == TT_IDENTIFICATOR) {
        //identificator
        symbol_t* id = var_table_find(last_token->str);
        if (id == NULL) {
            error("Reference to undefined variable", ERROR_SEM);
        } else if (id->def != true) {
            error("Using uninitialized variable in expression", ERROR_UNDEF);
        } else {
            //convert valid variable to expression
            expr.type = id->type;
            expr.addr = id->addr;
        }
    } else {
        //literal
        switch (last_token->type) {
            case TT_LIT_DOUBLE:
                expr.type = DOUBLE_LIT_DT;
                expr.double_val = last_token->double_val;
                break;
            case TT_LIT_INT:
                expr.type = INT_LIT_DT;
                expr.int_val = last_token->int_val;
                break;
            case TT_LIT_STRING:
                expr.type = STRING_LIT_DT;
                expr.str_val = last_token->str;
                break;
            default:
                warning("Wrong item in token buffer, probably error in rule aplication");
        }
    }
    vector_push(expr_buffer, expr);
}

#include "symbol.h"

bool is_equal_func(symbol_t* func1, symbol_t* func2);
bool is_equal_paramlist(ulist_str_t* paramList1, ulist_str_t* paramList2);
void load_builtin_functions();

void symbol_table_init() {
    var_table_stack = stack_init(SI_HTABLE);
    func_symtable = htable_init();
    load_builtin_functions();
}

symbol_t* var_table_find(str_t* name) {
    return stack_find(var_table_stack, name);
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
    stack_push(var_table_stack, curr_symtable);
}

void var_table_scope_exit() {
    htable_t* left_htable = stack_pop(var_table_stack);
    curr_symtable = stack_top(var_table_stack);
    ifj15_free(left_htable);
}

bool is_equal_paramlist(ulist_str_t* paramList1, ulist_str_t* paramList2) {
    unode_str_t* iter1 = paramList1->front;
    unode_str_t* iter2 = paramList2->front;
    while (iter1 != NULL || iter2 != NULL) {
        if(iter1->item.type != iter2->item.type)
            return false;
        iter1 = iter1->next;
        iter2 = iter2->next;
    }
    if (iter1 != NULL || iter2 != NULL)
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

void var_set_name(str_t* name) {
    bufferVar.name = name;
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
}

void func_set_name(str_t* name) {
    bufferFunc.name = name;
}
void func_add_param(symbol_t* paramSymbol) {
    if (bufferFunc.paramList == NULL) {
        bufferFunc.paramList = ulist_str_init();
    }
    if (paramSymbol->name == NULL
        || !ulist_get(bufferFunc.paramList, paramSymbol->name))
        ulist_set(bufferFunc.paramList, paramSymbol->name, *paramSymbol);
    else
        error("Two function parameters with the same name", ERROR_SEM);
}
void func_set_return_type(e_data_t retType) {
    bufferFunc.type = retType;
}

symbol_t* func_finish() {
    return &bufferFunc;
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

    var_init();

    func_init();
    func_set_return_type(INT_DT);
    func_set_name(lengthName);
    var_set_type(STRING_DT);
    func_add_param(var_finish());
    func_table_add(func_finish());

    func_init();
    func_set_return_type(STRING_DT);
    func_set_name(substrName);
    func_add_param(var_finish());
    var_set_type(INT_DT);
    func_add_param(var_finish());
    func_add_param(var_finish());
    func_table_add(func_finish());

    func_init();
    func_set_return_type(STRING_DT);
    func_set_name(concatName);
    var_set_type(STRING_DT);
    func_add_param(var_finish());
    func_add_param(var_finish());
    func_table_add(func_finish());

    func_init();
    func_set_return_type(INT_DT);
    func_set_name(findName);
    func_add_param(var_finish());
    func_add_param(var_finish());
    func_table_add(func_finish());

    func_init();
    func_set_return_type(STRING_DT);
    func_set_name(sortName);
    func_add_param(var_finish());
    func_table_add(func_finish());
}

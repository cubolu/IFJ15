#ifndef SYMBOL_H
#define SYMBOL_H

#include "common.h"

typedef enum {VAR_SYM, FUN_SYM} e_symbol_t;
typedef enum {NONE_DT, DOUBLE_DT = 13, INT_DT = 14, STRING_DT = 15,
              DOUBLE_LIT_DT, INT_LIT_DT, STRING_LIT_DT} e_data_t;

struct _symbol_t {
    str_t* name;
    e_symbol_t sym;
    e_data_t type; //also determines return type of function
    bool def; //also determines if var is initialized
    size_t addr;
    ulist_str_t* paramList;
};

struct _func_call_t {
    str_t* name;
    vector_int_t* paramTypes;
};

struct _expression_t {
    e_data_t type;
    union {
        size_t addr;
        str_t* str_val;
        int int_val;
        double double_val;
    };
};

#include "str.h"

symbol_t INVALID_SYM;

func_call_t bufferFuncCall;
symbol_t bufferFunc;
symbol_t bufferVar;

vector_htable_t* var_table_vector;

htable_t* curr_symtable;
htable_t* func_symtable;

void symbol_table_init();

symbol_t* var_table_find(str_t* name);
//symbol_t* var_table_curr_find(str_t* name);

void var_init();
void var_set_type(e_data_t type);
void var_set_name(str_t* name);
symbol_t* var_finish();

void func_init();
void func_set_name(str_t* name);
void func_add_param(symbol_t* paramSymbol);
void func_set_return_type(e_data_t retType);
symbol_t* func_finish();
//void func_init_code_addr();

void func_call_init();
void func_call_set_name(str_t* name);
void func_call_add_param(e_data_t type);
func_call_t* func_call_finish();

void var_table_add(symbol_t* symbol);
void var_table_scope_enter();
void var_table_scope_exit();

void func_table_add(symbol_t* symbol);
symbol_t* func_table_find(str_t* name);
bool is_equal_func(symbol_t* func1, symbol_t* func2);
bool is_valid_func_call(func_call_t* func_call, symbol_t* funcSym);

void check_rule_rel(token_t* operator, vector_expr_t* expr_buffer);
void check_rule_addsub(token_t* operator, vector_expr_t* expr_buffer);
void check_rule_muldiv(token_t* operator, vector_expr_t* expr_buffer);
void check_rule_id(token_t* last_token, vector_expr_t* expr_buffer);

#endif

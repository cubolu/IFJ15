#ifndef SYMBOL_H
#define SYMBOL_H

#include "common.h"

typedef enum {VAR_SYM, FUN_SYM} e_symbol_t;
typedef enum {DOUBLE_DT = 13, INT_DT = 14, STRING_DT = 15} e_data_t;

struct _symbol_t {
    str_t* name;
    e_symbol_t sym;
    e_data_t type; //also determines return type of function
    bool def; //also determines if var is initialized

    union {
        size_t addr;
        str_t* strval;
        int intval;
        double doubleval;
    };
    ulist_str_t* paramList;
};

#include "vector.h"
#include "str.h"

symbol_t INVALID_SYM;

symbol_t bufferFunc;
symbol_t bufferVar;
str_t dontCareName;

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

void var_table_add(symbol_t* symbol);
void var_table_scope_enter();
void var_table_scope_exit();

void func_table_add(symbol_t* symbol);
symbol_t* func_table_find(str_t* name);
bool is_equal_func(symbol_t* func1, symbol_t* func2);

#endif

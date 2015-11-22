#ifndef SYMBOL_H
#define SYMBOL_H

typedef enum {VAR_SYM, FUN_SYM} e_symbol_t;

struct _symbol_t {
    str_t* name;
    e_symbol_t sym_t;
    bool def; //also determines if var is initialized
    union {
        size_t addr;
        str_t* strval;
        int intval;
        double doubleval;
    };
};

symbol_t INVALID_SYM;

#endif

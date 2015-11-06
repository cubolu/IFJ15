#ifndef TOKEN_H
#define TOKEN_H

#include "str.h"

enum e_token_type {
    TT_VARIABLE_DECLARATION,

};

enum e_variable_type {PT_INT,
                    PT_STRING,
                    PT_DOUBLE,
                    PT_AUTO};


typedef struct {
    enum e_token_type type;
    union {
        struct {
           enum e_variable_type t;
           str * name;
        } variable_declaration;

        //deleni
        //operatory aritmeticky, relacni
        //integer
        //double
        //deklarace funkce
        //volani funkce
        //strednik
        //zacatek, konec bloku

    };

} token;

#endif // TOKEN_H


#include "common.h"
#include "memory.h"
#include "str.h"
#include "parser.h"

int main(int argc, const char *argv[]) {
    ifj15_memory_init();

    /*str * s = str_init();

    str_append_char(s, 'a');

    str_append_char(s, 'b');

    str_append_char(s, 'c');

    if (str_equals(s, "abc"))
        printf("%s\n", s->str);

    str_clear(s);

    str_append_char(s, '3');
    str_append_char(s, '.');
    str_append_char(s, '1');
    str_append_char(s, '4');

    printf("%g\n", str_to_double(s));*/


    parser * p = parser_init("test.c");

    while(1)
    {
        token_t tok = parser_next_token(p);

        printf("\nToken type: %d", tok.type);

        switch(tok.type)
        {
        case TT_VARIABLE_DECLARATION:
            printf(" VARIABLE_DECLARATION type: %d name: %s", tok.var_or_func_declaration.t, tok.var_or_func_declaration.name->str);
            break;

        case TT_INTEGER:
            printf(" INTEGER value: %d", tok.integer.value);
            break;

        case TT_DOUBLE:
            printf(" DOUBLE value: %g", tok.double_num.value);
            break;

        default:
            break;
        }

        printf("\n");

        if(tok.type == TT_EOF)
            break;
    }

    ifj15_free_all();
    return 0;
}

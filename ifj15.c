#include "common.h"
#include "memory.h"
#include "str.h"
#include "scanner.h"

int main(int argc, const char *argv[]) {
    ifj15_memory_init();

    scanner_t * p = scanner_init("test1");

    while(1)
    {
        token_t tok = next_token(p);

        printf("\nToken type: %d", tok.type);

        switch(tok.type)
        {
        case TT_IDENTIFICATOR:
            printf(" IDENTIFICATOR name: %s", tok.s->str);
            break;

        case TT_LIT_INT:
            printf(" INTEGER value: %d", tok.int_val);
            break;

        case TT_LIT_DOUBLE:
            printf(" DOUBLE value: %g", tok.double_val);
            break;

        case TT_LIT_STRING:
            printf(" STRING value: %s", tok.s->str);
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

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

    if (str_equals(s, "ab"))
        printf("%s\n", s->str);*/

    parser * p = parser_init("test.c");

    parser_next_token(p);

    ifj15_free_all();
    return 0;
}

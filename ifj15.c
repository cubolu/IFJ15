#include "common.h"
#include "memory.h"
#include "str.h"
#include "vector.h"
#include "scanner.h"
#include "parser.h"
#include "generator.h"
#include "interpreter.h"

int main(int argc, const char *argv[]) {
    ifj15_memory_init();

    if (argc < 2)
        error("Missing file operand", ERROR_INTERNAL);
    else if (argc > 2)
        warning("Too many file operands");

    FILE* input_file = ifj15_fopen(argv[1], "r");

    scanner_t* s = scanner_init(input_file);
    dLineNumInit(s);
    symbol_table_init();
    generator_init();
    parse_program(s);
    interpreter_init();
    run_program();

    ifj15_free_all();
    return 0;
}

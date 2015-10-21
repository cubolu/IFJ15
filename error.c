#include "error.h"

void error(const char *error_str, error_t error_type) {
    fprintf(stderr, "Error: %s", error_str);
    ifj15_free_all();
    exit(error_type);
}

void warning(const char *warn_str) {
    fprintf(stderr, "Warning: %s", warn_str);
}

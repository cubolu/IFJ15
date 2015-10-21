#include "common.h"
#include "memory.h"

int main(int argc, const char *argv[]) {
    ifj15_memory_init();
    
    ifj15_free_all();
    return 0;
}

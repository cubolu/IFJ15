# IFJ15(C++) Interpreter

Interpreter for the language IFJ15 which is a subset of C++ written in C.

### Contribution Guidelines *(Draft)*

- Functions that are not ment to be used outside of your component should be preceded with an `_` if they are visible through header files.
- There should be no no definitions of functions, only declarations, in header files. Unless they are inline.
- Every header file should include `common.h`.
- If you need an extra standard library, that wasn't yet included, include it also in `common.h`.
- If you are experiencing problems with cyclic header dependencies, this can usually be solved with a forward declaration in `common.h`.
- If you need to add a new data structure, which needs to be dynamically allocated and contains elements which need to be dynamically allocated, you should always add it for management into the memory management system instead of managing the allocations yourself. See [Memory Managment](doc/Memory.md) for more information.

### APIs and Guides

- [Memory Managment](doc/Memory.md)
- [Unit Testing](doc/Testing.md)
- [Data Structures](doc/Datastructures.md)
- [Error Handling](doc/Error.md)

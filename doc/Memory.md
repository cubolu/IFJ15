## Memory Managment

The memory management system in IFJ15 was designed so that you could exit the program at any point in your code **using the** `error()` **method** and the program will deallocate all dynamically allocated memory and exit. This allows for writing functions that don't have to check the return values of memory allocation functions and don't have to return values based on those functions. (For example when using nested calls, each of which could fail due to a memory allocation problem), but instead focus on returning meaningful values, pertaining to the meaning of the function.

### API

- `void* ifj15_malloc(ptr_t type, size_t size)`
  - Returns the pointer with allocated memory of `size` (in bytes). Requires `type` to be specified for the memory management system to know which deallocation routine to use.
- `void* ifj15_malloc(size_t size)`
  - Overload of the previous function, with the type set to the `SIMPLE` default. Use this if you didn't extend support for your type and/or if your type requires only a single `free()` call on the pointer.
- `void* ifj15_calloc(ptr_t type, size_t size)` or `void* ifj15_calloc(size_t size)`
  - Works exactly like `ifj15_malloc` but returns zero initialized memory.
- `void* ifj15_realloc(void* ptr, size_t size)`
  - Works exactly like system `realloc`, with the extra work of updating the global pointer table.
- `void ifj15_free(void* ptr)`
  - Frees memory at `ptr`. This method is the **only acceptable way** of deallocating memory allocated by `ifj15_*` methods. It updates the GPT. If the type of memory allocated at `ptr` is supported by the Memory Management system, this method only needs to be called once, on the top level pointer and it will deallocate the whole structure and it's "children".
- `void ifj15_free_all()`
  - This method **deallocates all dynamically allocated memory in the program**. Therefore it should only be used at the end of the program, that is, at the end of the `main` method.

### Allocating structures

Every data structure has a ``*_init()` function which works with the memory management system. It uses `ifj15_*` methods to allocate your structure, and add it to the GPT. You should always use this way of allocating data structures, for code clarity and convenience.

### Extending support for your structures

To add support for a new structure into the IFJ15 Memory Management System you need to:

1. Add your type to `enum_ptr_t` at `memory.h`
2. Write a `your_data_structure_init()` method which uses `ifj15_*` memory allocation methods to insert your structure into the GPT.
3. Write a `your_data_structure_free()` method and insert it to `_ifj15_free()` at 'memory.c'
4. Enjoy the benefits.

### Important tips

- All currently available values for 'ptr_t' can be found in 'memory.h' where the enum is defined.
- `your_data_structure_init()` **must only insert the top level pointer of your structure into the GPT.** Dynamically allocated sub-elements of your structure should be handled by your `_free` method without ever entering the GPT. This is to prevent unnecessary extra GPT entry overhead.
- You can use methods prefixed with `_` e.g. `_ifj15_malloc()` to implement your `*_init()` function. You will need to do this to override the `bool ptable_insert_b` option from the default `true` to `false` to make sure sub-elements don't get entered into the GPT.

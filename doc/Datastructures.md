## Data Structures

Data structures in IFJ15 are implemented to be as generic as possible. They accept keys and items of type `void*` (with exceptions) and don't care if you interpret that as an `int` or a pointer to some structure. **Warning:** This means that the data structures don't do type checking. They make no assumptions about the type of things you put in them and it's up to you to make sure they are homogeneous.

Every data structure has a `*_init()` function. You should use this function to initialize your data structure. Additional information on this can be found in the [Memory Management](/doc/Memory.md) section.

### API

#### Hash table

- `void htable_set(htable_t* htable, char* key, void* item)`
  - Inserts `item` into hash table `htable` under the key `key`. If an entry under the `key` already exists, it overrides it. So be careful with that.
- `void htable_get(htable_t* htable, char* key)`
  - Returns the element from `htable` under the key `key`. If there is no such element, returns `NULL`.
- `void htable_remove(htable_t* htable, char* key)`
  - Removes the element under key `key` from `htable`.
- `void* htable_pop(htable_t* htable, char* key)`
  - Returns the `item` saved under `key` in `htable` and removes the entry from the hash table.

#### Vector

*Note: Implemented as a resizable array for more uniform memory access.*

- `void vector_push_back(vector_t* vector, void* item)`
  - Inserts `item` at the end of `vector`.
- `void vector_push_front(vector_t* vector, void* item)`
  - Inserts `item` at the beginning of `vector`. **Warning:** This method shifts the indexes of your elements. Also, use `vector_push_back` in preference to this method, unless you have a reason otherwise, due to higher efficiency during resizing, if you only use `vector_push_back`.
- `void* vector_pop_back(vector_t* vector)`
  - Returns the last element of `vector` and removes it from `vector`. Returns `NULL` and a warning to `stderr` if you try to use this method on an empty vector.
- `void* vector_pop_front(vector_t* vector)`
  - Returns the first element of `vector` and removes it from `vector`. Returns `NULL` and a warning if you try to use this method on an empty vector.
- `void* vector_at(vector_t* vector, size_t pos)`
  - Random access method for the vector. Returns the element at `pos` in `vector`.


#### Pointer table

*Note: The pointer table differs from the hash table in that it's optimized to hash integers/pointers as keys instead of strings.*

- `void ptable_insert(ptable_t* ptable, void* ptr, void* item)`
  - Inserts `item` into the hash table `ptable` under the key `ptr`. If an entry under the `ptr` already exists, it overrides it. So be careful with that.
- `void* ptable_pop(ptable_t* ptable, char* key)`
  - Returns the `item` saved under `key` in `ptable` and removes the entry from the pointer table.

#### Unique Linked List

*Note: Used for implementation of Hash table and Pointer table. For most use cases, you should use Vector over ULL for greater efficiency.*

- `void ulist_set(ulist_t* ulist, void* key, void* item)`
  - Inserts `item` into the linked ulist `ulist` under key `key`.
  - This function has a nice trick (implemented using C11 \_Generic support), which is that if you pass it a `char*`, it uses a `strcmp` to compare the keys, otherwise defaults to `==`. If it's a `char*` it needs to be **a zero terminated char array.**
- `void* ulist_get(ulist_t* ulist, void* key)`
  - Returns `item` associated with `key` from `ulist` or `NULL` if the key is not inside the list. You need to check for that.
- `void* ulist_pop(ulist_t* ulist, void*/char* key)`
  - Returns the item associated with `key` from `ulist` and removes the entry from ulist. If no entry in `ulist` is associated with `key`, returns `NULL`.

### Important Tips

- **All of the data structures in IFJ15 save keys and items by reference.** This means you need to dynamically allocate, and deallocate the keys and items separately from the data structures (unless they are global / can be contained in a void* directly) for them to remain accessible once they leave scope of where they were inserted into the data structures.

## Data Structures

Data structures in IFJ15 are implemented to be as generic as possible. They accept keys and items of type `void*` (with exceptions) and don't care if you interpret that as an `int` or a pointer to some structure. **Warning:** This means that the data structures don't do type checking. They make no assumptions about the type of things you put in them and it's up to you to make sure they are homogeneous.

Every data structure has a `*_init()` function. You should use this function to initialize your data structure. Additional information on this can be found in the [Memory Management](doc/Memory.md) section.

### API

#### Hash table

- `void htable_insert(htable_t* htable, char* key, void* item)`
  - Inserts `item` into hash table `htable` under the key `key`. If an entry under the `key` already exists, it overrides it. So be careful with that.
- `void* htable_pop(htable_t* htable, char* key)`
  - Returns the `item` saved under `key` in `htable` and removes the entry from the hash table.

#### Pointer table

*Note: The pointer table differs from the hash table in that it's optimized to hash integers/pointers as keys instead of strings.*

- `void ptable_insert(ptable_t* ptable, void* ptr, void* item)`
  - Inserts `item` into the hash table `ptable` under the key `ptr`. If an entry under the `ptr` already exists, it overrides it. So be careful with that.
- `void* ptable_pop(ptable_t* ptable, char* key)`
  - Returns the `item` saved under `key` in `ptable` and removes the entry from the pointer table.

#### Linked List

- `void list_insert(list_t* list, void* key, void* item)`
  - Inserts `item` into the linked list `list` under key `key`.
- `void* list_pop(list_t* list, void*/char* key)`
  - Returns the item associated with `key` from `list` and removes the entry from list. If multiple items are saved with the same key, it returns the first occurrence in the order of insertion.
  - This function has a nice trick (implemented using C11 \_Generic support), which is that if you pass it a `char*`, it uses a `strcmp` to compare the keys, otherwise defaults to `==`. If it's a `char*` it needs to be **a zero terminated char array.**

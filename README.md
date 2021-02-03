# map
A type-safe generic hashmap implementation for C.

## Installation 
The [map.c](src/cmap.c?raw=1) and [map.h](src/cmap.h?raw=1) files can be dropped
into an existing C project and compiled along with it.
You can also git clone it and add_subdirectory() from cmake project, then link to cmap library.


## Usage
Before using a map it should first be initialised using the `map_init()`
function.
```c
map_t(key_type, value_type) m;
map_init(&m, map_generic_cmp, map_generic_hash);
```

Values can added to a map using the `map_set()` function.
```c
map_set(&m, "testkey", 123);
```

To retrieve a value from a map, the `map_get()` function can be used.
`map_get()` will return a pointer to the key's value, or `NULL` if no mapping
for that key exists.

```c
int *val = map_get(&m, "testkey");
if (val) {
  printf("value: %d\n", *val);
} else {
  printf("value not found\n");
}
```

When you are done with a map the `map_delete()` function should be called on
it. This will free any memory the map allocated during use.
```c
map_delete(&m);
```

To define a new map type the `map_t()` macro should be used:
```c
/* Creates the type uint_map_t for storing unsigned ints */
typedef map_t(signed char, unsigned int) uint_map_t;
```

## Functions
All map functions are macro functions. The parameter `m` in each function
should be a pointer to the map struct which the operation is to be performed
on. The `key` parameter should always be a first type passed to map_t.

### map\_t(KT, VT)
Creates a map struct for containing keys of type `KT` and values of type `VT`.
```c
/* Typedefs the struct `ptr_fp_map_t` as a container for type FILE* */
typedef map_t(void *, FILE*) ptr_fp_map_t;
```

### map\_init(m, key_cmp_func, key_hash_func)
Initialises the map, this must be called before the map can be used. 
There's two four default sets of functions you can use:
  - map_string_cmp 
  - map_string_hash
---
  - map_generic_cmp
  - map_generic_hash

`_generic_` functions can be used with any basic key types like int, float, long double, etc.
If you use complex types like structs or unions, you should provide own compatible functions. \
`_string_` functions should be used if char * as a key type is a null-terminated string.

### typedef size_t (*MapHashFunction)(const void *key, size_t memsize);
Where memsize is sizeof(KT)

### typedef int (*MapCmpFunction)(const void *a, const void *b, size_t ksize);
Should return 0 if both objects pointed to by a and b are equal, otherwise >0 if a > b and <0 if b > a.
Where ksize is sizeof(KT), but can be ignored if the actual size is known 
(for example when key type is an array or a nul-terminated string).

### map\_delete(m)
Deinitialises the map, freeing the memory the map allocated during use;
this should be called when we're finished with a map.

### map\_get(m, key)
Returns a pointer to the value of the given `key`. If no mapping for the `key`
exists then `NULL` will be returned.

### map\_set(m, key, value)
Sets the given `key` to the given `value`. Returns `1` on success, otherwise
`0` is returned and the map remains unchanged.

### map\_remove(m, key)
Removes the mapping of the given `key` from the map. If the `key` does not
exist in the map then the function has no effect.

### map\_iter(m)
Returns a `map_iter_t` which can be used with `map_next()` to iterate all the
keys in the map.


### map\_next(m, iter, obj_ptr)
Uses the `map_iter_t` returned by `map_iter()` to iterate all the keys in the
map. `map_next()` returns a key with each call and returns `NULL` when there
are no more keys.
```c
int key;
map_iter_t iter = map_iter(&m);

while (map_next(&m, &iter, &key)) {
  printf("%d -> %d", key, *map_get(&m, key));
}
```

## Known bugs(implementation disadvantages)
`map_remove` on current node key will cause freed memory access bug when values are iterated
```c
const char *key;
map_iter_t iter = map_iter(&m);

while ((key = map_next(&m, &iter))) {
  map_remove(&m, key); // access violation error on bound check enabled debuggers(MSVC), freed memory is accessed, key ptr is no longer valid
}
```



## License
This library is free software; you can redistribute it and/or modify it under
the terms of the MIT license. See [LICENSE](LICENSE) for details.

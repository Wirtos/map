#include "src/map.h" // map_*
#include <stdio.h> // printf
#include <stdlib.h> // malloc, EXIT_*

// Turn size_t into dynamic string representation, needs free call afterwards
char *itoa_dyn(size_t num) {
    char const digit[] = "0123456789";

    size_t shifter = num, c = 1; // 1 is already reserved for \0
    do { // Move to the end of integer representation
        c++;
        shifter = shifter / 10;
    } while (shifter);
    void *alloc = malloc(sizeof(char) * c); // Create string big enough to hold number

    if (alloc == NULL) { // Alloc failed, return NULL
        return NULL;
    }
    // Init pointer pointing to the last char array element where last element is size - 1
    char *str_ptr = &((char *) alloc)[c - 1];
    *str_ptr = '\0';

    do { // Iterate backwards & set values
        *(--str_ptr) = digit[num % 10];
        num = num / 10;
    } while (num);

    return str_ptr;
}

int main(void) {
    map_t(size_t) map;
    map_init(&map);
    printf("MAP INIT\n");
    char *key = NULL;

    for (size_t i = 0; i != 10; i++) {
        key = itoa_dyn(i); // Turn size_t key into char array
        if (key == NULL) { // Handle malloc error
            printf("Failed itoa_dyn alloc");
            goto fail;
        }

        if (map_set(&map, key, i + 1) != 0) { // Handle setter error, status 0 == OK
            printf("Failed map_set alloc on key %zu\n", i);
            goto fail;
        }

        size_t *val = map_get(&map, key);

        if (val == NULL) {
            // Handle getter error, NULL pointer returned if there's no such key(shouldn't be possible in this example)
            printf("Failed map_get alloc on key %zu\n", i);
            goto fail;
        }
        printf("key: \"%s\" value: %zu\n", key, *val);
        free(key); // Free itoa_dyn key result
    }
    printf("\nIterate key by key:\n"); // Notice the fact that keys aren't sorted
    map_iter_t iter = map_iter(&map);
    const char *iter_key;
    while ((iter_key = map_next(&map, &iter))) {
        printf("key: \"%s\" got, value: %zu\n", iter_key, *map_get(&map, iter_key));
        printf("key: \"%s\" removed!\n", iter_key);
        map_remove(&map, iter_key); // Remove & free key & value
    }
    map_delete(&map); // Free entire map
    printf("MAP FREED\n");
    return EXIT_SUCCESS;

    fail:
    map_delete(&map); // Free entire map
    printf("MAP FREED\n");
    free(key); // Free itoa_dyn key result
    return EXIT_FAILURE;
}
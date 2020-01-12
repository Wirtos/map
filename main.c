#include "src/map.h" /* map_**/
#include <stdio.h> /* printf*/
#include <stdlib.h> /* malloc, EXIT_**/

int main(void) {
    map_t(size_t) map; /* define custom map with size_t value type*/
    map_init(&map);
    printf("MAP INIT\n");
    char *keys[] = {"k0", "k1", "k2", "k3", "k4", "k5", "k6", "k7", "k8", "k9"};

    for (size_t i = 0; i < 10; i++) {
        if (!map_set(&map, keys[i], i + 1)) { /* Handle setter error*/
            printf("Failed map_set alloc on key %zu\n", i);
            goto fail;
        }

        size_t *val = map_get(&map, keys[i]);

        if (val == NULL) {
            /* Handle getter error, NULL pointer returned if there's no such key (shouldn't be possible in this example)*/
            printf("Failed map_get on key %zu\n", i);
            goto fail;
        }
        printf("'%s' -> %zu\n", keys[i], *val);
    }
    printf("\nIterate key by key:\n"); /* Notice the fact that keys aren't sorted*/
    const char *iter_key;

    map_iter_t get_iter = map_iter(&map);
    while ((iter_key = map_next(&map, &get_iter))) {
        size_t ret = *map_get(&map, iter_key);
        printf("'%s' -> %zu\n", iter_key, ret);

    }

    map_delete(&map); /* Free entire map*/
    printf("MAP FREED\n");
    return EXIT_SUCCESS;

    fail:
    map_delete(&map); /* Free entire map*/
    printf("MAP FREED\n");
    return EXIT_FAILURE;
}
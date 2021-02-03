#include <assert.h>
#include <cmap.h>   /* map_* */
#include <stdio.h>  /* printf */
#include <stdlib.h> /* EXIT_* */

int main(void) {
    unsigned i;
    char *keys[] = {"k0", "k1", "k2", "k3", "k4", "k5", "k6", "k7", "k8", "k9"};
    map_t(char *, unsigned) map; /* define custom map */
    map_init(&map, map_string_cmp, map_string_hash);
    printf("MAP INIT\n");

    for (i = 0; i < 10; i++) {
        if (!map_set(&map, keys[i], i + 1)) { /* Handle setter error*/
            printf("Failed map_set alloc on key %u\n", i);
            goto fail;
        }
        {
            unsigned *val = map_get(&map, keys[i]);

            if (val == NULL) {
                /* Handle getter error, NULL pointer returned if there's no such key (shouldn't be possible in this example)*/
                printf("Failed map_get on key %u\n", i);
                goto fail;
            }
            printf("'%s' -> %u\n", keys[i], *val);
        }
    }
    map_remove(&map, keys[0]);
    {
        char *iter_key;
        map_iter_t get_iter = map_iter(&map);
        printf("\nIterate key by key:\n"); /* Notice the fact that keys aren't sorted*/

        while ((map_next(&map, &get_iter, &iter_key))) {
            unsigned ret = *map_get(&map, iter_key);
            printf("'%s' -> %u\n", iter_key, ret);

        }
    }

    map_delete(&map); /* Free entire map*/
    printf("MAP FREED\n");
    return EXIT_SUCCESS;

    fail:
    map_delete(&map); /* Free entire map*/
    printf("MAP FREED\n");
    return EXIT_FAILURE;
}

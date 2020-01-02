#include "src/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

char *itoa_dyn(int i) {
    char const digit[] = "0123456789";

    const int sign_needed = i < 0;

    if (sign_needed) {
        i *= -1;
    }

    size_t shifter = i, c = 0;
    do { //Move to where representation ends
        c++;
        shifter = shifter / 10;
    } while (shifter);
    c = c + 1 + sign_needed; // +1 for \0
    void *alloc = malloc(sizeof(char) * c);
    if (alloc == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    char *p = &((char *) alloc)[--c];
    *p = '\0';

    do { //Move back, inserting digits as u go
        *--p = digit[i % 10];
        i = i / 10;

    } while (i);
    if (sign_needed) {
        *--p = '-';
    }
    return p;
}

int main() {
    map_t(size_t) map;
    map_init(&map);
    int failed_alloc = 0;
    for(size_t i =0; i!=SIZE_MAX;i++) {
        char * key = itoa_dyn(i);
        map_set(&map, key, i);

        size_t *val = map_get(&map, key);
        free(key);
        if (!val) {
            printf("failed alloc on %d\n", i);
            break;
        }

    }

    map_delete(&map);
    return 0;
}
/** 
 * Copyright (c) 2020 Wirtos
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <stdlib.h>
#include <string.h>
#include "map.h"

struct map_node_t {
    size_t hash;
    void *value;
    map_node_t *next;
    /* char key[]; */
    /* char value[]; */
};


static size_t map_hash(const char *str) {
    size_t hash = 5381;
    while (*str) {
        hash = ((hash << 5) + hash) ^ *str++;
    }
    return hash;
}


static map_node_t *map_newnode(const char *key, void *value, size_t vsize) {
    map_node_t *node;
    size_t ksize = strlen(key) + 1;
    size_t voffset = ksize + ((sizeof(void *) - ksize) % sizeof(void *));
    node = (map_node_t *) malloc(sizeof(*node) + voffset + vsize);
    if (node == NULL) {
        return NULL;
    }
    memcpy(node + 1, key, ksize);
    node->hash = map_hash(key);
    node->value = ((char *) (node + 1)) + voffset;
    memcpy(node->value, value, vsize);
    return node;
}


size_t map_bucketidx(map_base_t *m, size_t hash) {
    /* If the implementation is changed to allow a non-power-of-2 bucket count,
     * the line below should be changed to use mod instead of AND */
    return hash & (m->nbuckets - 1);
}


static void map_addnode(map_base_t *m, map_node_t *node) {
    size_t n = map_bucketidx(m, node->hash);
    node->next = m->buckets[n];
    m->buckets[n] = node;
}


static short map_resize(map_base_t *m, size_t nbuckets) {
    map_node_t *nodes, *node, *next;
    map_node_t **buckets;
    size_t i;
    /* Chain all nodes together */
    nodes = NULL;
    i = m->nbuckets;
    while (i--) {
        node = (m->buckets)[i];
        while (node) {
            next = node->next;
            node->next = nodes;
            nodes = node;
            node = next;
        }
    }
    /* Reset buckets */
    buckets = (map_node_t **)realloc(m->buckets, sizeof(*m->buckets) * nbuckets);
    if (buckets != NULL) {
        m->buckets = buckets;
        m->nbuckets = nbuckets;
    }
    if (m->buckets) {
        memset(m->buckets, 0, sizeof(*m->buckets) * m->nbuckets);
        /* Re-add nodes to buckets */
        node = nodes;
        while (node) {
            next = node->next;
            map_addnode(m, node);
            node = next;
        }
    }
    /* Return error code if realloc() failed */
    return (buckets == NULL) ? -1 : 0;
}


static map_node_t **map_getref(map_base_t *m, const char *key) {
    size_t hash = map_hash(key);
    map_node_t **next;
    if (m->nbuckets > 0) {
        next = &m->buckets[map_bucketidx(m, hash)];
        while (*next) {
            if ((*next)->hash == hash && !strcmp((char *) (*next + 1), key)) {
                return next;
            }
            next = &(*next)->next;
        }
    }
    return NULL;
}


void map_delete_(map_base_t *m) {
    map_node_t *next, *node;
    size_t i;
    i = m->nbuckets;
    while (i--) {
        node = m->buckets[i];
        while (node) {
            next = node->next;
            free(node);
            node = next;
        }
    }
    free(m->buckets);
}


void *map_get_(map_base_t *m, const char *key) {
    map_node_t **next = map_getref(m, key);
    return next ? (*next)->value : NULL;
}


short map_set_(map_base_t *m, const char *key, void *value, size_t vsize) {
    short err;
    size_t n;
    map_node_t **next, *node;
    /* Find & replace existing node */
    next = map_getref(m, key);
    if (next) {
        memcpy((*next)->value, value, vsize);
        return 0;
    }
    /* Add new node */
    node = map_newnode(key, value, vsize);
    if (node == NULL) {
        goto fail;
    }
    if (m->nnodes >= m->nbuckets) {
        n = (m->nbuckets > 0) ? (m->nbuckets << 1) : 1;
        err = map_resize(m, n);
        if (err) {
            goto fail;
        }
    }
    map_addnode(m, node);
    m->nnodes++;
    return 0;
    fail:
    if (node) {
        free(node);
    }
    return -1;
}


void map_remove_(map_base_t *m, const char *key) {
    map_node_t *node;
    map_node_t **next = map_getref(m, key);
    if (next) {
        node = *next;
        *next = (*next)->next;
        free(node);
        m->nnodes--;
    }
}


map_iter_t map_iter_(void) {
    map_iter_t iter;
    iter.bucketidx = -1;
    iter.node = NULL;
    return iter;
}


const char *map_next_(map_base_t *m, map_iter_t *iter) {
    if (iter->node) {
        iter->node = iter->node->next;
        if (iter->node == NULL) {
            goto nextBucket;
        }
    } else {
        nextBucket:
        do {
            if (++iter->bucketidx >= m->nbuckets) {
                return NULL;
            }
            iter->node = m->buckets[iter->bucketidx];
        } while (iter->node == NULL);
    }
    return (char *) (iter->node + 1);
}

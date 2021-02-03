/** 
 * Copyright (c) 2020 Wirtos
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <stdlib.h> /* malloc, realloc*/
#include <string.h> /* memcpy, strlen, strcmp*/
#include "cmap.h"

struct map_node_t {
    size_t hash;
    void *key;
    void *value;
    map_node_t *next;
};

/* djb2 hashing algorithm */
size_t map_generic_hash(const void *mem, size_t bsize) {
    /*5381 and 32 - efficient magic numbers*/
    const unsigned char *barr = mem;
    size_t hash = 5381;
    size_t i;
    for (i = 0; i < bsize; i++) {
        hash = ((hash * 33) + hash) ^ (barr[i]);
    }
    return hash;
}

size_t map_string_hash(const void *mem, size_t bsize) {
    const unsigned char *barr = mem;
    size_t hash = 5381;
    (void) bsize;
    while (*barr){
        hash = ((hash * 33) + hash) ^ (*barr++);
    }
    return hash;
}

int map_generic_cmp(const void *a, const void *b, size_t ksize) {
    return memcmp(a, b, ksize);
}

int map_string_cmp(const void *a, const void *b, size_t ksize) {
    (void) ksize;
    return strcmp(*(const char **) a, *(const char **) b);
}

static void map_freenode(map_node_t *node){
    free(node->key);
    free(node->value);
    free(node);
}

static map_node_t *map_newnode(const void *key, size_t ksize, const void *value, size_t vsize, MapHashFunction hash_func) {
    map_node_t *node;
    node = (map_node_t *) malloc(sizeof(*node));
    if (node == NULL) {
        return NULL;
    }
    node->key = NULL;
    node->value = NULL;

    node->key = malloc(ksize);
    if (node->key == NULL) goto fail;
    memcpy(node->key, key, ksize);
    node->hash = hash_func(key, ksize); /* Call map-specific hash function */
    node->next = NULL;

    node->value = malloc(vsize);
    if (node->value == NULL) goto fail;
    memcpy(node->value, value, vsize);
    return node;
fail:
    map_freenode(node);
    return NULL;
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


static char map_resize(map_base_t *m, size_t nbuckets) {
    map_node_t *nodes, *node, *next;
    map_node_t **buckets;
    size_t i;
    /* Chain all nodes together */
    nodes = NULL;
    i = m->nbuckets;
    while (i--) {
        node = (m->buckets)[i];
        while (node != NULL) {
            next = node->next;
            node->next = nodes;
            nodes = node;
            node = next;
        }
    }
    /* Reset buckets */
    buckets = (map_node_t **) realloc(m->buckets, sizeof(*m->buckets) * nbuckets);
    if (buckets != NULL) {
        m->buckets = buckets;
        m->nbuckets = nbuckets;
        memset(m->buckets, 0, sizeof(*m->buckets) * m->nbuckets);
        /* Re-add nodes to buckets */
        node = nodes;
        while (node != NULL) {
            next = node->next;
            map_addnode(m, node);
            node = next;
        }
    }
    /* Return error code if realloc() failed */
    return (buckets == NULL) ? 0 : 1;
}


static map_node_t **map_getref(map_base_t *m, const void *key, size_t ksize) {
    size_t hash = m->hash_func(key, ksize);
    map_node_t **next;
    if (m->nbuckets > 0) {
        next = &m->buckets[map_bucketidx(m, hash)];
        while (*next != NULL) {
            if ((*next)->hash == hash && m->cmp_func(key, (*next)->key, ksize) == 0) {
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
        while (node != NULL) {
            next = node->next;
            map_freenode(node);
            node = next;
        }
    }
    free(m->buckets);
}


void *map_get_(map_base_t *m, const void *key, size_t ksize) {
    map_node_t **next = map_getref(m, key, ksize);
    return next != NULL ? (*next)->value : NULL;
}


char map_set_(map_base_t *m, const void *key, size_t ksize, const void *value, size_t vsize) {
    size_t n;
    map_node_t **next, *node;
    /* Find & replace existing node */
    next = map_getref(m, key, ksize);
    if (next != NULL) {
        memcpy((*next)->value, value, vsize);
        return 1;
    }
    /* Add new node */
    node = map_newnode(key, ksize, value, vsize, m->hash_func);
    if (node == NULL) {
        goto fail;
    }
    if (m->nnodes >= m->nbuckets) {
        n = (m->nbuckets > 0) ? (m->nbuckets * 2) : 1;
        if (!map_resize(m, n)) {
            goto fail;
        }
    }
    map_addnode(m, node);
    m->nnodes++;
    return 1;
    fail:
    if (node != NULL) {
        map_freenode(node);
    }
    return 0;
}


void map_remove_(map_base_t *m, const void *key, size_t ksize) {
    map_node_t *node;
    map_node_t **next = map_getref(m, key, ksize);
    if (next != NULL) {
        node = *next;
        *next = (*next)->next;
        map_freenode(node);
        m->nnodes--;
    }
}


map_iter_t map_iter_(void) {
    map_iter_t iter;
    iter.bucketidx = -1;
    iter.node = NULL;
    return iter;
}


void *map_next_(map_base_t *m, map_iter_t *iter) {
    if (iter->node != NULL) {
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
    return iter->node->key;
}

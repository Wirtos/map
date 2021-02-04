/*************************************************************************
 * Copyright (c) 2020 Wirtos
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef CMAP_H
#define CMAP_H

#include <string.h> /* memset */

#define MAP_VER_MAJOR 1
#define MAP_VER_MINOR 1
#define MAP_VER_PATCH 0

typedef size_t (*MapHashFunction)(const void *key, size_t memsize);
typedef int (*MapCmpFunction)(const void *a, const void *b, size_t memsize);

typedef struct map_node_t map_node_t;

typedef struct {
    map_node_t **buckets;
    size_t nbuckets, nnodes;
    MapHashFunction hash_func;
    MapCmpFunction cmp_func;
} map_base_t;

typedef struct {
    size_t bucketidx;
    map_node_t *node;
} map_iter_t;


#define map_t(KT, VT)\
  struct { map_base_t base; KT tmpkey; KT *keyref; VT tmpval; VT *valref;}


#define map_init(m, key_cmp_func, key_hash_func)\
  memset(m, 0, sizeof(*m)), \
        (m)->base.buckets = NULL, \
        (m)->base.cmp_func = (key_cmp_func != NULL) ? key_cmp_func : map_generic_cmp, \
        (m)->base.hash_func = (key_hash_func != NULL) ? key_hash_func : map_generic_hash


#define map_delete(m)\
  map_delete_(&(m)->base)


#define map_get(m, key) \
  ((m)->tmpkey = key,                      \
  (m)->valref = map_get_(&(m)->base, &(m)->tmpkey, sizeof((m)->tmpkey)))


#define map_set(m, key, value)\
  ((m)->tmpval = (value), (m)->tmpkey = (key),\
    map_set_(&(m)->base, &(m)->tmpkey, sizeof((m)->tmpkey), &(m)->tmpval, sizeof((m)->tmpval)))

#define map_remove(m, key) \
    ((m)->tmpkey = (key),  \
     map_remove_(&(m)->base, &(m)->tmpkey, sizeof((m)->tmpkey)))

#define map_iter(m) \
  map_iter_()

#define map_next(m, iter, kptr)                 \
    ((m)->keyref = map_next_(&(m)->base, iter), \
     ((m)->keyref)                              \
         ? ((*kptr = *(m)->keyref), 1)          \
         : 0)

#define map_equal(m1, m2, val_cmp_func)               \
    (((void)((1) ? &(m1)->tmpkey : &(m2)->tmpkey)), \
     ((void)((1) ? &(m1)->tmpval : &(m2)->tmpval)), \
     map_equal_(&(m1)->base, &(m2)->base, sizeof((m2)->tmpkey), sizeof((m2)->tmpval), (val_cmp_func)))


size_t map_generic_hash(const void *mem, size_t bsize);

size_t map_string_hash(const void *mem, size_t bsize);

int map_generic_cmp(const void *a, const void *b, size_t ksize);

int map_string_cmp(const void *a, const void *b, size_t ksize);


void map_delete_(map_base_t *);

void *map_get_(map_base_t *, const void *, size_t);

char map_set_(map_base_t *, const void *, size_t, const void *, size_t);

void map_remove_(map_base_t *, const void *, size_t);

map_iter_t map_iter_(void);

void *map_next_(map_base_t *, map_iter_t *);

int map_equal_(map_base_t *m1, map_base_t *m2, size_t ksize, size_t vsize, MapCmpFunction val_cmp_func);

/*typedef map_t(void *) map_void_t;
typedef map_t(char *) map_str_t;
typedef map_t(int) map_int_t;
typedef map_t(char) map_char_t;
typedef map_t(float) map_float_t;
typedef map_t(double) map_double_t;*/

#endif /* CMAP_H */

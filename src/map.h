/**
 * Copyright (c) 2020 Wirtos
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef MAP_H
#define MAP_H

#include <string.h> /* memset*/

#define MAP_VERSION "0.1.2"


typedef struct map_node_t map_node_t;

struct map_node_t {
    size_t hash;
    void *value;
    map_node_t *next;
};

typedef struct {
    map_node_t **buckets;
    size_t nbuckets, nnodes;
} map_base_t;

typedef struct {
    size_t bucketidx;
    map_node_t *node;
} map_iter_t;


#define map_t(T)\
  struct { map_base_t base; T *ref; T tmp; }


#define map_init(m)\
  memset(m, 0, sizeof(*m))


#define map_delete(m)\
  map_delete_(&(m)->base)


#define map_get(m, key)\
  ((m)->ref = map_get_(&(m)->base, key))


#define map_set(m, key, value)\
  ((m)->tmp = (value),\
    map_set_(&(m)->base, key, &(m)->tmp, sizeof((m)->tmp)))


#define map_remove(m, key)\
  map_remove_(&(m)->base, key)


#define map_iter(m)\
  map_iter_()


#define map_next(m, iter)\
  map_next_(&(m)->base, iter)


void map_delete_(map_base_t *);

void *map_get_(map_base_t *, const char *);

char map_set_(map_base_t *, const char *, void *, size_t);

void map_remove_(map_base_t *, const char *);

map_iter_t map_iter_(void);

const char *map_next_(map_base_t *, map_iter_t *);


typedef map_t(void *) map_void_t;
typedef map_t(char *) map_str_t;
typedef map_t(int) map_int_t;
typedef map_t(char) map_char_t;
typedef map_t(float) map_float_t;
typedef map_t(double) map_double_t;

#endif

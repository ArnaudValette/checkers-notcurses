#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/*
╰┭━╾┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅╼━┮╮
╭╯ datastructures > dynamic arrays                                          ╭╯╿
╙╼━╾┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄━━╪*/

#define DYN_ARR_INITIAL_SIZE 128

typedef struct dyn_arr {
  void **data;
  unsigned int start;
  unsigned int end;
  size_t capacity;
} dyn_arr;

static dyn_arr *dyn_arr_create() {
  dyn_arr *a = malloc(sizeof(dyn_arr));
  if (!a) return NULL;
  a->capacity = DYN_ARR_INITIAL_SIZE;
  a->data = malloc(a->capacity * sizeof(void *));
  if (!a->data) {
    free(a);
    return NULL;
  }
  a->start = a->end = 0;
  return a;
}

static inline int dyn_arr_is_empty(dyn_arr *a) { return a->end == a->start; }

static size_t dyn_arr_len(dyn_arr *a) {
  return (a->end + a->capacity - a->start) % a->capacity;
}

static void *dyn_arr_get(dyn_arr *a, size_t i) {
  if (i >= dyn_arr_len(a)) {
    return NULL;
  }
  return a->data[(a->start + i) % a->capacity];
}

static void dyn_arr_destroy(dyn_arr *a) {
  free(a->data);
  a->data = NULL;
  free(a);
  a = NULL;
}

static inline int __dyn_arr_full(dyn_arr *a) {
  return (a->end + 1) % a->capacity == a->start;
}

static inline int __dyn_arr_resize(dyn_arr *a) {
  if (a->capacity > (SIZE_MAX) / 2 / sizeof(void *)) {
    return 1;
  }
  size_t len = dyn_arr_len(a);
  size_t new_cap = a->capacity * 2;

  void **data = malloc(new_cap * sizeof(void *));
  if (!data) return 1;

  for (size_t i = 0; i < len; i++) {
    data[i] = a->data[(a->start + i) % a->capacity];
  }
  free(a->data);
  a->data = data;
  a->capacity = new_cap;
  a->start = 0;
  a->end = len;
  return 0;
}

/** @brief append to the end of the array */
static int dyn_arr_append(dyn_arr *a, void *data) {
  if (__dyn_arr_full(a)) {
    if (__dyn_arr_resize(a)) {
      return 1;
    }
  }
  a->data[a->end] = data;
  a->end = (a->end + 1) % a->capacity;
  return 0;
}

/** @return the first element of the array (removed). */
static void *dyn_arr_pop(dyn_arr *a) {
  if (dyn_arr_is_empty(a)) {
    return NULL;
  }
  void *v = a->data[a->start];
  a->start = (a->start + 1) % a->capacity;
  return v;
}

/** @return the last element of the array (removed). */
static void *dyn_arr_pop_last(dyn_arr *a) {
  if (dyn_arr_is_empty(a)) {
    return NULL;
  }
  a->end = (a->end + a->capacity - 1) % a->capacity;
  return a->data[a->end];
}

/** @brief push to the start of the array */
static int dyn_arr_push(dyn_arr *a, void *data) {
  if (__dyn_arr_full(a)) {
    if (__dyn_arr_resize(a)) {
      return 1;
    }
  }
  a->start = (a->start + a->capacity - 1) % a->capacity;
  a->data[a->start] = data;
  return 0;
}

/*
╰┭━╾┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅╼━┮╮
╭╯ datastructures > hashmap                                                 ╭╯╿
╙╼━╾┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄━━╪*/

#define HASHMAP(kType, vType, name)\
typedef struct name{ \
  kType key; \
  vType value; \
  struct name *next; \
} name; \
typedef struct{ \
  name **entries; \
} name##_map;

/*
╰┭━╾┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅╼━┮╮
╭╯ datastructures > branching system                                        ╭╯╿
╙╼━╾┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄━━╪*/

#endif

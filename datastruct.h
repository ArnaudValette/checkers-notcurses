#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#include <cstddef>
#include <cstdint>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define DEQUE_INITIAL_SIZE 128

#ifndef DATASTRUCT_USE_VOID_BASED_DEQUE

/*
╰┭━╾┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅╼━┮╮
╭╯ datastructures § array deque → macro implementation                      ╭╯╿
╙╼━╾┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄━━╪*/

#define DEQUE_TYPEDECL(type, name) \
  typedef struct{\
    type **data; \
    size_t start; \
    size_t end; \
    size_t capacity; \
} name##_deque;

#define DEQUE_IMPL(type, name) \
static name##_deque * name##_deque_create() { \
  name##_deque *a = malloc(sizeof(name##_deque)); \
  if (!a) return NULL; \
  a->capacity = DEQUE_INITIAL_SIZE; \
  a->data = malloc(a->capacity * sizeof(type *)); \
  if (!a->data) { \
    free(a); \
    return NULL; \
  } \
  a->start = a->end = 0; \
  return a; \
} \
static inline int name##_deque_is_empty(name##_deque *a) { return a->end == a->start; }\
 \
static size_t name##_deque_len(name##_deque *a) { \
  return (a->end + a->capacity - a->start) % a->capacity; \
} \
 \
static type * name##_deque_get(name##_deque *a, size_t i) { \
  if (i >= name##_deque_len(a)) { \
    return NULL; \
  } \
  return a->data[(a->start + i) % a->capacity]; \
} \
 \
static void name##_deque_destroy(name##_deque *a) { \
  free(a->data); \
  a->data = NULL; \
  free(a); \
  a = NULL; \
} \
 \
static inline int name##__deque_full(name##_deque *a) { \
  return (a->end + 1) % a->capacity == a->start; \
} \
 \
static inline int name##__deque_resize(name##_deque *a) { \
  if (a->capacity > (SIZE_MAX) / 2 / sizeof(type *)) { \
    return 1; \
  } \
  size_t len = name##_deque_len(a); \
  size_t new_cap = a->capacity * 2; \
 \
  type **data = malloc(new_cap * sizeof(type *)); \
  if (!data) return 1; \
 \
  for (size_t i = 0; i < len; i++) { \
    data[i] = a->data[(a->start + i) % a->capacity]; \
  } \
  free(a->data); \
  a->data = data; \
  a->capacity = new_cap; \
  a->start = 0; \
  a->end = len; \
  return 0; \
} \
 \
/** @brief append to the end of the array */ \
static int name##_deque_append(name##_deque *a, type *data) { \
  if (name##__deque_full(a)) { \
    if (name##__deque_resize(a)) { \
      return 1; \
    } \
  } \
  a->data[a->end] = data; \
  a->end = (a->end + 1) % a->capacity; \
  return 0; \
} \
 \
/** @return the first element of the array (removed). */ \
static type * name##_deque_pop(name##_deque *a) { \
  if (name##_deque_is_empty(a)) { \
    return NULL; \
  } \
  type *v = a->data[a->start]; \
  a->start = (a->start + 1) % a->capacity; \
  return v; \
} \
 \
/** @return the last element of the array (removed). */ \
static type * name##_deque_pop_last(name##_deque *a) { \
  if (name##_deque_is_empty(a)) { \
    return NULL; \
  } \
  a->end = (a->end + a->capacity - 1) % a->capacity; \
  return a->data[a->end]; \
} \
 \
/** @brief push to the start of the array */ \
static int name##_deque_push(name##_deque *a, type *data) { \
  if (name##__deque_full(a)) { \
    if (name##__deque_resize(a)) { \
      return 1; \
    } \
  } \
  a->start = (a->start + a->capacity - 1) % a->capacity; \
  a->data[a->start] = data; \
  return 0; \
}

#else

/*
╰┭━╾┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅╼━┮╮
╭╯ datastructures § array deque → (void *) implementation                   ╭╯╿
╙╼━╾┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄━━╪*/

typedef struct deque {
  void **data;
  size_t start;
  size_t end;
  size_t capacity;
} deque;

static deque *deque_create() {
  deque *a = malloc(sizeof(deque));
  if (!a) return NULL;
  a->capacity = DEQUE_INITIAL_SIZE;
  a->data = malloc(a->capacity * sizeof(void *));
  if (!a->data) {
    free(a);
    return NULL;
  }
  a->start = a->end = 0;
  return a;
}

static inline int deque_is_empty(deque *a) { return a->end == a->start; }

static size_t deque_len(deque *a) {
  return (a->end + a->capacity - a->start) % a->capacity;
}

static void *deque_get(deque *a, size_t i) {
  if (i >= deque_len(a)) {
    return NULL;
  }
  return a->data[(a->start + i) % a->capacity];
}

static void deque_destroy(deque *a) {
  free(a->data);
  a->data = NULL;
  free(a);
  a = NULL;
}

static inline int __deque_full(deque *a) {
  return (a->end + 1) % a->capacity == a->start;
}

static inline int __deque_resize(deque *a) {
  if (a->capacity > (SIZE_MAX) / 2 / sizeof(void *)) {
    return 1;
  }
  size_t len = deque_len(a);
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
static int deque_append(deque *a, void *data) {
  if (__deque_full(a)) {
    if (__deque_resize(a)) {
      return 1;
    }
  }
  a->data[a->end] = data;
  a->end = (a->end + 1) % a->capacity;
  return 0;
}

/** @return the first element of the array (removed). */
static void *deque_pop(deque *a) {
  if (deque_is_empty(a)) {
    return NULL;
  }
  void *v = a->data[a->start];
  a->start = (a->start + 1) % a->capacity;
  return v;
}

/** @return the last element of the array (removed). */
static void *deque_pop_last(deque *a) {
  if (deque_is_empty(a)) {
    return NULL;
  }
  a->end = (a->end + a->capacity - 1) % a->capacity;
  return a->data[a->end];
}

/** @brief push to the start of the array */
static int deque_push(deque *a, void *data) {
  if (__deque_full(a)) {
    if (__deque_resize(a)) {
      return 1;
    }
  }
  a->start = (a->start + a->capacity - 1) % a->capacity;
  a->data[a->start] = data;
  return 0;
}

#endif

/*
╰┭━╾┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅╼━┮╮
╭╯ datastructures § hashmap → macro based implementation                    ╭╯╿
╙╼━╾┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄━━╪*/

#define HASHMAP(kType, vType, name)\
typedef struct name{ \
  kType key; \
  vType value; \
  struct name *next; \
} name; \
typedef struct{ \
  name **entries; \
  uint64_t seed; \
} \
name##_map;

/*
╰┭━╾┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅╼━┮╮
╭╯ datastructures § hashmap → (void *) based implementation                 ╭╯╿
╙╼━╾┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄━━╪*/

/* *(hashmap+hash("yourKey")) */
typedef struct entry {
  char *key;
  void *value;
  struct entry *next;
} entry;

typedef struct {
  entry **buckets;
  uint64_t seed;
} hashmap;

/*
╰┭━╾┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅╼━┮╮
╭╯ datastructures § hashmap → hash functions                                ╭╯╿
╙╼━╾┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄━━╪*/

/* xxHash 64-bit like */
static const uint64_t PRIME64_1 = 0x9E3779B185EBCA87ull;
static const uint64_t PRIME64_2 = 0xC2B2AE3D27D4EB4Full;
static const uint64_t PRIME64_3 = 0x165667B19E3779F9ull;
static const uint64_t PRIME64_4 = 0x85EBCA77C2B2AE63ull;
static const uint64_t PRIME64_5 = 0x27D4EB2F165667C5ull;

static inline uint64_t datastruct_rotl_64(uint64_t n, uint8_t d) {
  return (n << d) | (n >> (64 - d));
}

static inline uint64_t datastruct_round(uint64_t acc, uint64_t lane) {
  acc = acc + (lane * PRIME64_2);
  acc = datastruct_rotl_64(acc, 31);
  return acc * PRIME64_1;
}

static inline uint64_t datastruct_merge(uint64_t dest, uint64_t src) {
  dest = dest ^ datastruct_round(dest, src);
  dest = dest * PRIME64_1;
  return dest + PRIME64_4;
}

static uint64_t datastruct_hash(void *data, size_t len, uint64_t seed) {
  uint64_t acc;
  uint64_t lanes[4];
  size_t cursor = 0;
  uint8_t *d = (uint8_t *)data;

  if (len < 32) {
    acc = seed + PRIME64_5;
  } else {
    /* 2. Stripes processing */
    uint64_t acc1 = seed + PRIME64_1 + PRIME64_2;
    uint64_t acc2 = seed + PRIME64_2;
    uint64_t acc3 = seed + 0;
    uint64_t acc4 = seed - PRIME64_1;
    while (len >= 32) {
      /* Each lane reads 64 bits */
      for (int i = 0; i < 4; i++) {
        lanes[i] = (d[cursor] << 7) | (d[cursor + 1] << 6) |
                   (d[cursor + 2] << 5) | (d[cursor + 3] << 4) |
                   (d[cursor + 4] << 3) | (d[cursor + 5] << 2) |
                   (d[cursor + 6] << 1) | (d[cursor + 7]);
        cursor += 8;
      }
      acc1 = datastruct_round(acc1, lanes[0]);
      acc2 = datastruct_round(acc2, lanes[1]);
      acc3 = datastruct_round(acc3, lanes[2]);
      acc4 = datastruct_round(acc4, lanes[3]);
    }
    /* 3. Accumulators merging */
    acc = datastruct_rotl_64(acc1, 1) + datastruct_rotl_64(acc2, 7) +
          datastruct_rotl_64(acc3, 12) + datastruct_rotl_64(acc4, 18);
    acc = datastruct_merge(acc, acc1);
    acc = datastruct_merge(acc, acc2);
    acc = datastruct_merge(acc, acc3);
    acc = datastruct_merge(acc, acc4);
  }
  /* Meet-up */
  acc = acc + len;
  while (len >= 8) {
    lanes[0] = (d[cursor] << 7) | (d[cursor + 1] << 6) | (d[cursor + 2] << 5) |
               (d[cursor + 3] << 4) | (d[cursor + 4] << 3) |
               (d[cursor + 5] << 2) | (d[cursor + 6] << 1) | (d[cursor + 7]);
    acc = acc ^ datastruct_round(acc, lanes[0]);
    acc = datastruct_rotl_64(acc, 27) * PRIME64_1;
    acc = acc + PRIME64_4;
    cursor += 8;
    len -= 8;
  }
  if (len >= 4) {
    lanes[0] = (d[cursor] << 3) | (d[cursor + 1] << 2) | (d[cursor + 2] << 1) |
               (d[cursor + 3]);
    acc = acc ^ (lanes[0] * PRIME64_1);
    acc = datastruct_rotl_64(acc, 23) * PRIME64_2;
    acc = acc + PRIME64_3;
    cursor += 4;
    len -= 4;
  }
  while (len >= 1) {
    lanes[0] = d[cursor++];
    acc = acc ^ (lanes[0] * PRIME64_5);
    acc = datastruct_rotl_64(acc, 11) * PRIME64_1;
    len--;
  }
  acc = acc & (acc >> 33);
  acc = acc * PRIME64_2;
  acc = acc ^ (acc >> 29);
  acc = acc * PRIME64_3;
  acc = acc ^ (acc >> 32);
  return acc;
}

/*
╰┭━╾┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅╼━┮╮
╭╯ datastructures § branching system → implementation                       ╭╯╿
╙╼━╾┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄━━╪*/
/*
╰┭━╾┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅╼━┮╮
╭╯ datastructures § branching system → documentation                        ╭╯╿
╙╼━╾┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄━━╪*/

#endif

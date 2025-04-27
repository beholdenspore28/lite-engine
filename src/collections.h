#ifndef SC_LIST_H
#define SC_LIST_H

#include <stdio.h>
#include <stdlib.h>

#define SC_LIST_INITIAL_CAPACITY (128)
#define sc_foreach(i, cap) for (unsigned int i = 0; i < cap; i++)

typedef size_t sc_list_size;

typedef struct {
  sc_list_size capacity;
  sc_list_size count;
} sc_list_meta_data;

#define SC_LIST(type)                                                          \
  typedef type *sc_list_##type;                                                \
                                                                               \
  static inline sc_list_size sc_list_##type##_count(                           \
      const sc_list_##type list) {                                             \
    sc_list_meta_data *data = ((sc_list_meta_data *)(list)) - 1;               \
    return data->count;                                                        \
  }                                                                            \
                                                                               \
  static inline sc_list_##type sc_list_##type##_alloc(void) {                  \
    void *ptr = malloc(sizeof(sc_list_meta_data) +                             \
                       (sizeof(type) * SC_LIST_INITIAL_CAPACITY));             \
    sc_list_meta_data *data = (sc_list_meta_data *)ptr;                        \
    data->capacity = SC_LIST_INITIAL_CAPACITY;                                 \
    data->count = 0;                                                           \
                                                                               \
    return (sc_list_##type)(data + 1);                                         \
  }                                                                            \
                                                                               \
  static inline void sc_list_##type##_free(sc_list_##type list) {              \
    sc_list_meta_data *data = ((sc_list_meta_data *)(list)) - 1;               \
    free(data);                                                                \
  }                                                                            \
                                                                               \
  static inline void sc_list_##type##_add(sc_list_##type *list,                \
                                          const type element) {                \
    sc_list_meta_data *data = ((sc_list_meta_data *)(*list)) - 1;              \
    if (data->count >= data->capacity) {                                       \
      const sc_list_size new_capacity = data->count * 2 + 1;                   \
      data = realloc(data, sizeof(sc_list_meta_data) +                         \
                               (sizeof(type) * new_capacity));                 \
      *list = (sc_list_##type)(data + 1);                                      \
    }                                                                          \
    (*list)[data->count] = element;                                            \
    data->count++;                                                             \
  }                                                                            \
                                                                               \
  static inline sc_list_##type sc_list_##type##_alloc_from_array(              \
      sc_list_##type other, sc_list_size length) {                             \
    sc_list_##type l = sc_list_##type##_alloc();                               \
    sc_foreach(i, length) { sc_list_##type##_add(&l, other[i]); }              \
    return l;                                                                  \
  }                                                                            \
                                                                               \
  static inline void sc_list_##type##_remove_at(const sc_list_##type list,     \
                                                const sc_list_size index) {    \
    if (!list)                                                                 \
      return;                                                                  \
    sc_list_meta_data *data = ((sc_list_meta_data *)(list)) - 1;               \
    (list)[index] = (list)[data->count - 1];                                   \
    data->count--;                                                             \
  }

#endif // SC_LIST_H

#include "vec_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct vec_list{
  size_t size;
  size_t max_size;
  vector_t **array;
} vec_list_t;

vec_list_t *vec_list_init(size_t initial_size){
  vec_list_t *new_list = malloc(sizeof(vec_list_t));
  assert(new_list != NULL);
  new_list->array = malloc(initial_size* sizeof(vector_t));
  assert(new_list->array != NULL);
  new_list->max_size = initial_size;
  new_list->size = 0;
  return new_list;
}

void vec_list_free(vec_list_t *list){
  for (size_t i = 0; i < list->size; i++) {
    free(list->array[i]);
  }
  free(list->array);
  free(list);
}

size_t vec_list_size(vec_list_t *list){
  return list->size;
}

vector_t *vec_list_get(vec_list_t *list, size_t index){
  assert(index < list->size);
  return list->array[index];
}

void vec_list_add(vec_list_t *list, vector_t *value){
  assert(list->size < list->max_size);
  assert(value != NULL);
  list->array[list->size] = value;
  list->size = list->size + 1;
}

vector_t *vec_list_remove(vec_list_t *list){
  assert(list->size != 0);
  vector_t *removed = list->array[list->size - 1];
  list->size = list->size - 1;
  return removed;
}

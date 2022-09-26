#include "vector.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>
#include <assert.h>
const size_t growth_factor = 2;
void list_resize(list_t *list);

typedef struct list{
  void **data;
  size_t size;
  size_t capacity;
  free_func_t freedom;
} list_t;

list_t *list_init(size_t initial_size, free_func_t freer){
  list_t *list = malloc(sizeof(list_t));
  assert(list != NULL);
  assert(initial_size > 0);
  list->data = malloc(sizeof(void *) * initial_size);
  assert(list->data != NULL);
  list->capacity = initial_size;
  list->size = 0;
  list->freedom = freer;
  return list;
}

void list_free(list_t *list){
  for(size_t i = 0; i < list->size; i++){
    list->freedom(list->data[i]);
  }
  free(list->data);
  free(list);
}

size_t list_size(list_t *list){
  return list->size;
}

void* list_get(list_t *list, size_t index){
  size_t size = list_size(list);
  assert (size > index);
  return list->data[index];
}

void list_add(list_t *list, void *value){
  size_t size = list_size(list);
  assert(value != NULL);
  if (size == list->capacity){
    list_resize(list);
  }
  list->data[size] = value;
  list->size++;
}

void list_resize(list_t *list){
  list_t *temp = list_init(list_size(list) * growth_factor, free);
  for (size_t i = 0; i < list_size(list); i++){
    list_add(temp, list_get(list, i));
  }
  *list = *temp;
  free(temp);
}


void *list_remove(list_t *list, size_t index){
  size_t size = list_size(list);
  assert(index < size);
  void *removed = list->data[index];
  for (size_t i = index; i < size - 1; i++){
    list->data[i] = list->data[i + 1];
  }
  // list->freedom(list->data[size - 1]);
  list->size--;
  return removed;
}

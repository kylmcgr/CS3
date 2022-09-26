#include "scene.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>

const int original_bodies = 100;

typedef struct scene{
  list_t *data;
} scene_t;

scene_t *scene_init(void){
  scene_t *new_scene = malloc(sizeof(scene_t));
  assert(new_scene != NULL);
  new_scene->data = list_init(original_bodies, free);
  return new_scene;
}

void scene_free(scene_t *scene){
  list_free(scene->data);
  free(scene);
}

size_t scene_bodies(scene_t *scene){
  return list_size(scene->data);
}

body_t *scene_get_body(scene_t *scene, size_t index){
  return list_get(scene->data, index);
}

void scene_add_body(scene_t *scene, body_t *body){
  list_add(scene->data, body);
}

void scene_remove_body(scene_t *scene, size_t index){
  list_remove(scene->data, index);
}

void scene_tick(scene_t *scene, double dt){
  for (size_t i = 0; i < list_size(scene->data); i++){
    body_tick(list_get(scene->data, i), dt);
  }
}

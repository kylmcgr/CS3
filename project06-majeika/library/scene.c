#include "scene.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>


const int original_bodies = 100;

typedef struct aux{
  force_creator_t force;
  void *aux;
  free_func_t freer;
  list_t *bodies;
} aux_t;

typedef struct scene{
  list_t *data;
  list_t *force_creators;
} scene_t;

 void aux_free(aux_t *aux){
   if (aux->bodies != NULL){
     list_free(aux->bodies);
   }
   if(aux->freer != NULL){
     aux->freer(aux->aux);
   }
   free(aux);
 }

scene_t *scene_init(void){
  scene_t *new_scene = malloc(sizeof(scene_t));
  assert(new_scene != NULL);
  new_scene->data = list_init(original_bodies, (free_func_t) body_free);
  new_scene->force_creators = list_init(original_bodies, (free_func_t) aux_free);
  return new_scene;
}

void scene_free(scene_t *scene){
  list_free(scene->data);
  list_free(scene->force_creators);
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
  body_t *body_to_remove = scene_get_body(scene, index);
  body_remove(body_to_remove);
}

void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux, free_func_t freer){
  scene_add_bodies_force_creator(scene, forcer, aux, NULL, freer);
}

void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer, void *aux, list_t *bodies, free_func_t freer){
  aux_t *newForce = malloc(sizeof(aux_t));
  newForce->force = forcer;
  newForce->aux = aux;
  newForce->freer = freer;
  newForce->bodies = bodies;
  list_add(scene->force_creators, newForce);
}

void scene_tick(scene_t *scene, double dt){
  for (size_t i = 0; i < list_size(scene->force_creators); i++){
    aux_t *force_creator = list_get(scene->force_creators, i);
    force_creator_t force = force_creator->force;
    void *force_aux = force_creator->aux;
    force(force_aux);
  }
  for (size_t i = list_size(scene->data); i > 0; i--){
    body_t *body = list_get(scene->data, i - 1);
    if (body_is_removed(body)){
      for (size_t j = list_size(scene->force_creators); j > 0; j--){
        aux_t *aux = list_get(scene->force_creators, j - 1);
        list_t *bodies = aux->bodies;
        if (bodies != NULL){
          for (size_t k = 0; k < list_size(bodies); k++){
            if (list_get(bodies, k) == body){
              aux_free(list_remove(scene->force_creators, j - 1));
              break;
            }
          }
        }
      }
      body_free(list_remove(scene->data, i - 1));
    } else {
      body_tick(body, dt);
    }
  }
}

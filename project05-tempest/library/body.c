#include "body.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>


const vector_t centered_loc = {0, 500};
const double TRANSLATION_CONST = 0.5;

typedef struct body{
  list_t *shape;
  void *info;
  double mass;
  vector_t centroid;
  vector_t velocity;
  rgb_color_t color;
  vector_t force;
  vector_t impulse;
  free_func_t info_freer;
  bool removed;
} body_t;

body_t *body_init(list_t *shape, double mass, rgb_color_t color){
  body_t *new_shape = body_init_with_info(shape, mass, color, NULL, NULL);
  return new_shape;
}

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color, void *info, free_func_t info_freer){
  body_t *new_shape = malloc(sizeof(body_t));
  assert(new_shape != NULL);
  new_shape->shape = shape;
  assert(new_shape->shape != NULL);
  assert(mass > 0);
  new_shape->mass = mass;
  new_shape->centroid = polygon_centroid(shape);
  new_shape->color = color;
  vector_t velocity = {0.0, 0.0};
  new_shape->velocity = velocity;
  vector_t force = {0.0, 0.0};
  new_shape->force = force;
  vector_t impulse = {0.0, 0.0};
  new_shape->impulse = impulse;
  new_shape->removed = false;
  new_shape->info = info;
  new_shape->info_freer = info_freer;
  return new_shape;
}

list_t *body_copy(list_t *list){
  list_t *copy = list_init(list_size(list), free);
  for (size_t i = 0; i < list_size(list); i++){
    vector_t *element = list_get(list, i);
    vector_t *copy_elem = malloc(sizeof(vector_t));
    copy_elem->x = element->x;
    copy_elem->y = element->y;
    list_add(copy, copy_elem);
  }
  return copy;
}

double body_get_mass(body_t *body){
  return body->mass;
}

list_t *body_get_shape(body_t *body){
  return body_copy(body->shape);
}

vector_t body_get_centroid(body_t *body){
  return body->centroid;
}

vector_t body_get_velocity(body_t *body){
  return body->velocity;
}

void body_free(body_t *body){
  list_free(body->shape);
  if(body->info_freer != NULL){
    body->info_freer(body->info);
  }
  free(body);
}

rgb_color_t body_get_color(body_t *body){
  return body->color;
}

void *body_get_info(body_t *body){
  return body->info;
}

void body_set_centroid(body_t *body, vector_t x){
  vector_t translate = vec_subtract(x, body->centroid);
  polygon_translate(body->shape, translate);
  body->centroid = x;
}

void body_set_velocity(body_t *body, vector_t v){
  body->velocity = v;
}

void body_set_rotation(body_t *body, double angle){
  polygon_rotate(body->shape, angle, body->centroid);
}

void body_add_force(body_t *body, vector_t force){
  body->force = vec_add(body->force, force);
}

void body_add_impulse(body_t *body, vector_t impulse){
  body->impulse = vec_add(body->impulse, impulse);
}

void body_tick(body_t *body, double dt){
  // finding the acceleration and setting the new velocity
  //if (body->mass != INFINITY){
    double inv_mass = 1 / body->mass;
    vector_t acceleration = vec_multiply(inv_mass, body->force);
    vector_t added_vel = vec_multiply(dt, acceleration);
    vector_t mass_impulse = vec_multiply(inv_mass, body->impulse);
    added_vel = vec_add(added_vel, mass_impulse); //is this right?
    vector_t new_velocity = vec_add(body->velocity, added_vel);
    vector_t translate = vec_add(body->velocity, new_velocity);
    translate = vec_multiply(TRANSLATION_CONST * dt, translate);
    vector_t centroid = vec_add(body_get_centroid(body), translate);
    body_set_centroid(body, centroid);
    body->velocity = new_velocity;
  //}
  // reset the forces and impulse
  vector_t force = {0.0, 0.0};
  body->force = force;
  vector_t impulse = {0.0, 0.0};
  body->impulse = impulse;
}

void body_remove(body_t *body){
  body->removed = true;
}

bool body_is_removed(body_t *body){
  return body->removed;
}

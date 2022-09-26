#include "body.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>


const vector_t centered_loc = {0, 500};

typedef struct body{
  list_t *shape;
  double mass;
  vector_t centroid;
  vector_t velocity;
  rgb_color_t color;
} body_t;

body_t *body_init(list_t *shape, double mass, rgb_color_t color){
  body_t *new_shape = malloc(sizeof(body_t));
  assert(new_shape != NULL);
  new_shape->shape = shape;
  assert(new_shape->shape != NULL);
  new_shape->mass = mass;
  new_shape->centroid = polygon_centroid(shape);
  new_shape->color = color;
  vector_t velocity = {0.0, 0.0};
  new_shape->velocity = velocity;
  return new_shape;
}

list_t *body_copy(list_t *list){
  list_t *copy = list_init(list_size(list), free);
  for (int i = 0; i < list_size(list); i++){
    vector_t *element = list_get(list, i);
    vector_t *copy_elem = malloc(sizeof(vector_t));
    copy_elem->x = element->x;
    copy_elem->y = element->y;
    list_add(copy, copy_elem);
  }
  return copy;
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

void body_free(body_t *figure){
  list_free(figure->shape);
  free(figure);
}

rgb_color_t body_get_color(body_t *figure){
  return figure->color;
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

void body_tick(body_t *body, double dt){
  vector_t translate = body->velocity;
  translate.x *= dt;
  translate.y *= dt;
  vector_t centroid = vec_add(body_get_centroid(body), translate);
  body_set_centroid(body, centroid);
}

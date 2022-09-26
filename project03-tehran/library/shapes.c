#include "shapes.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>


const vector_t centered_loc = {0, 500};
const int color_upper = 1;
const int color_lower = 0;

typedef struct shape{
  list_t *vectors;
  vector_t velocity;
  float red;
  float green;
  float blue;
} shape_t;

shape_t *shape_init(size_t point_size, vector_t velocity, float red, float green,
  float blue){
  shape_t *new_shape = malloc(sizeof(shape_t));
  new_shape->vectors = list_init(point_size, free);
  assert(new_shape->vectors != NULL);
  new_shape->velocity = velocity;
  new_shape->red = red;
  new_shape->green = green;
  new_shape->blue = blue;
  return new_shape;
}

list_t *get_vectors(shape_t *figure){
  return figure->vectors;
}

vector_t get_vel(shape_t *figure){
  return figure->velocity;
}

void shape_free(shape_t *figure){
  list_free(figure->vectors);
  free(figure);
}

float color(){
  float saturation = (float)(rand() % (color_upper - color_lower + 1)) + color_lower;
  return saturation;
}

float get_red(shape_t *figure){
  return figure->red;
}

float get_green(shape_t *figure){
  return figure->green;
}

float get_blue(shape_t *figure){
  return figure->blue;
}

void set_vectors(shape_t *figure, list_t* vec){
  figure->vectors = vec;
}

void set_velocity(shape_t *figure, vector_t vel){
  figure->velocity.x = vel.x;
  figure->velocity.y = vel.y;
}

//Creates a list_t for a polygon with a set number of points
list_t* draw_polygon(size_t list_size){
  list_t *points = list_init(list_size, free);
  double radius_outer = 70;
  vector_t initial = {0, radius_outer};

  //fining angle between points
  double theta = (2 * M_PI) / list_size;
  for (size_t i = 0; i < list_size / 2; i++){
    vector_t *new_out = malloc(sizeof(vector_t));
    vector_t *new_in = malloc(sizeof(vector_t));
    initial = vec_rotate(initial, theta);
    *new_out = initial;

    //adding outer point
    list_add(points, new_out);
    initial = vec_multiply(.4, initial);
    initial = vec_rotate(initial, theta);
    *new_in = initial;

    //adding inner point
    list_add(points, new_in);
    initial = vec_multiply(2.5, initial);
  }
  polygon_translate(points, centered_loc);
  return points;
}

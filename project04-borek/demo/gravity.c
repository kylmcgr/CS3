#include "shapes.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>

const double gravity = -8000;
const vector_t min = {0, 0};
const vector_t max = {1000.0, 500.0};
const double rotation = .005;
const vector_t vel_num = {400.0, 400.0};
const double vel_scalar = .9;


void animate(list_t *polygons, double dt);
void calculate_v(shape_t *figure, double dt);
rgb_color_t random_color();

rgb_color_t random_color(){
  rgb_color_t color;
  color.r = ((float) rand() / ((float) RAND_MAX));
  color.b = ((float) rand() / ((float) RAND_MAX));
  color.g = ((float) rand() / ((float) RAND_MAX));
  return color;
}

 int main(){
  vector_t vel = vel_num;
  size_t point_size = 2;
  size_t capacity = 1;
  double time_counter = 1;
  sdl_init(min, max);
  list_t *polygons = list_init(capacity, (free_func_t)shape_free);
  scene_t *scene = scene_init();
  // animates the graphic
  while(sdl_is_done(scene) != true){
    double dt = time_since_last_tick();
    time_counter += dt;

    // adds new shape
    if (time_counter >= 1){
      point_size += 2;
      shape_t *figure = shape_init(point_size, vel, color(), color(), color());
      list_t *vec = draw_polygon(point_size);
      set_vectors(figure, vec);
      list_add(polygons, figure);
      time_counter = 0;
    }
    sdl_clear();
    animate(polygons, dt);
    sdl_show();
  }
  list_free(polygons);
  return 0;
}
// moves and rotates the individual shapes in one time-step
void animate(list_t *polygons, double dt){
  for (size_t i = 0; i < list_size(polygons); i++){
    shape_t *poly_i = (shape_t*)list_get(polygons, i);
    list_t *poly_i_vec = get_vectors(poly_i);
    vector_t center_poly = polygon_centroid(poly_i_vec);
    calculate_v(poly_i, dt);
    vector_t vel = get_vel(poly_i);
    vector_t translation = {vel.x * dt, vel.y * dt};
    polygon_translate(poly_i_vec, translation);
    polygon_rotate(poly_i_vec, rotation, center_poly);
    sdl_draw_polygon(poly_i_vec, random_color());
  }
}
//calculates the veloicty after hitting bottom edge
void calculate_v(shape_t *figure, double dt){
  bool check = false;
  list_t *vec = get_vectors(figure);
  vector_t vel = get_vel(figure);
  for (size_t i = 0; i < list_size(vec); i++){
    vector_t curr = *(vector_t*)list_get(vec, i);
    if (vel.y < 0){
      if (curr.y < min.y){
        check = true;
      }
    }
  }
  if (check == true){
    vel.y = (-1) * vel.y * vel_scalar;
  }
  vel.y = vel.y + (gravity * dt);
  set_velocity(figure, vel);
}

#include "body.h"
#include "sdl_wrapper.h"
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
const int RADIUS_OUTER_NUM = 70;
const double OUTER = .4;
const double INNER = 2.5;
const double initial_x = 100;
const double initial_y = 100;


rgb_color_t random_color(){
  rgb_color_t color;
  color.r = ((float) rand() / ((float) RAND_MAX));
  color.b = ((float) rand() / ((float) RAND_MAX));
  color.g = ((float) rand() / ((float) RAND_MAX));
  return color;
}

list_t* draw_polygon(size_t list_size){
  list_t *points = list_init(list_size, free);
  double radius_outer = RADIUS_OUTER_NUM;
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
    initial = vec_multiply(OUTER, initial);
    initial = vec_rotate(initial, theta);
    *new_in = initial;

    //adding inner point
    list_add(points, new_in);
    initial = vec_multiply(INNER, initial);
  }
  vector_t centered_loc = (vector_t) {WINDOW_WIDTH - initial_x, WINDOW_HEIGHT - initial_y};
  polygon_translate(points, centered_loc);
  return points;
}

//calculates the veloicty after hitting bottom edge
void calculate_v(body_t *figure, double dt){
  bool check = false;
  list_t *vec = body_get_shape(figure);
  vector_t vel = body_get_velocity(figure);
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
  body_set_velocity(figure, vel);
}
// moves and rotates the individual shapes in one time-step
void animate(list_t *polygons, double dt){
  for (size_t i = 0; i < list_size(polygons); i++){
    body_t *poly_i = (body_t*)list_get(polygons, i);
    list_t *poly_i_vec = body_get_shape(poly_i);
    vector_t center_poly = polygon_centroid(poly_i_vec);
    calculate_v(poly_i, dt);
    vector_t vel = body_get_velocity(poly_i);
    vector_t translation = {vel.x * dt, vel.y * dt};
    polygon_translate(poly_i_vec, translation);
    polygon_rotate(poly_i_vec, rotation, center_poly);
    sdl_draw_polygon(poly_i_vec, random_color());
  }
}

 int main(){
  vector_t vel = vel_num;
  size_t point_size = 2;
  size_t capacity = 1;
  double time_counter = 1;
  sdl_init(min, max);
  list_t *polygons = list_init(capacity, (free_func_t)body_free);
  scene_t *scene = scene_init();
  // animates the graphic
  while(sdl_is_done(scene) != true){
    double dt = time_since_last_tick();
    time_counter += dt;

    // adds new shape
    if (time_counter >= 1){
      point_size += 2;
      list_t *vec = draw_polygon(point_size);
      body_t *figure = body_init(vec, 1, random_color());
      body_set_velocity(figure, vel);
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

#include "sdl_wrapper.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "body.h"

const char *WINDOW_TITLE = "CS 3";
const double WINDOW_WIDTH = 1000.0;
const double WINDOW_HEIGHT = 500.0;
const double rotation_rate = M_PI/1000;
const double initial_vx = 200.0;
const double initial_vy = 0.0;
const double initial_starx = 100.0;
const double initial_stary = WINDOW_HEIGHT - 100;
const double initial_star_pointx = 0.0;
const double initial_star_pointy = 100.0;
const double star_point_length = 50.0;
const double elasticity_factor = 0.8;
const double gravity = -900.8;

rgb_color_t random_color(){
  rgb_color_t color;
  color.r = (double) rand() / (RAND_MAX);
  color.g = (double) rand() / (RAND_MAX);
  color.b = (double) rand() / (RAND_MAX);
  return color;
}

bool compute_new_position(body_t *star, int star_points, double dt){
  vector_t curr_velocity = body_get_velocity(star);
  double xvelocity = curr_velocity.x;
  double yvelocity = curr_velocity.y;
  double to_translate_y = 0.0;
  bool stop_drawing = true;
  body_set_rotation(star, rotation_rate);
  for (int i = 0; i < star_points * 2; i++){
    list_t *shape = body_get_shape(star);
    vector_t *curr = list_get(shape, i);
    if (curr->x < WINDOW_WIDTH) {
      stop_drawing = false;
    }
    if (curr->y < 0 && yvelocity<0) {
      if (curr->y < to_translate_y) {
        to_translate_y = curr->y;
      }
    }
    list_free(shape);
  }
  vector_t new_velocity = {xvelocity, yvelocity};
  if (to_translate_y != 0) {
    new_velocity.y = -new_velocity.y * elasticity_factor;
  }
  new_velocity.y += gravity * dt;
  body_set_velocity(star, new_velocity);
  return stop_drawing;
}

body_t *make_star(int star_points) {
  list_t *shape = list_init(star_points*2, free);
  for (int i = 0; i < star_points * 2; i++) {
    vector_t *new = malloc(sizeof(vector_t));
    assert(new != NULL);
    new->x = initial_star_pointx;
    new->y = initial_star_pointy;
    if ((i % 2) == 0){
      new->y = star_point_length;
    }
    double newx = new->x * cos(M_PI/star_points * i) - new->y * sin(M_PI/star_points * i);
    double newy = new->x * sin(M_PI/star_points * i) + new->y * cos(M_PI/star_points * i);
    new->x = initial_starx+newx;
    new->y = initial_stary+newy;
    list_add(shape, new);
  }
  body_t *new_star = body_init(shape, 1, random_color());
  vector_t velocity = {initial_vx, initial_vy};
  body_set_velocity(new_star, velocity);
  return new_star;
}

int main (int argc, char *argv[]) {
  double increment = 1.0;
  int star_points = 2;
  int curr_size = 10;
  int last_star = 0;
  vector_t topRight = {WINDOW_WIDTH, WINDOW_HEIGHT};
  vector_t bottomLeft = {0.0, 0.0};
  sdl_init(bottomLeft, topRight);
  list_t *stars = list_init(curr_size, (free_func_t)body_free);
  double dt = 0.0;
  while (!sdl_is_done()) {
    if (increment >= 1.0) {
      increment -= 1.0;
      list_add(stars, make_star(star_points));
      star_points++;
    }
    dt = time_since_last_tick();
    increment += dt;
    for (size_t i = last_star; i < list_size(stars); i++) {
      body_tick(list_get(stars, i), dt);
      if (compute_new_position(list_get(stars, i), i + 2, dt)) {
        last_star++;
      }
    }
    sdl_clear();
    for (size_t i = last_star; i < list_size(stars); i++) {
      body_t *star = list_get(stars, i);
      rgb_color_t color = body_get_color(star);
      list_t *shape = body_get_shape(star);
      sdl_draw_polygon(shape, color.r, color.g, color.b);
      list_free(shape);
    }
    sdl_show();
  }
  list_free(stars);
}

#include "sdl_wrapper.h"
#include <math.h>
#include "polygon.h"
#include <stdlib.h>
#include <assert.h>

const char *WINDOW_TITLE = "CS 3";
const double WINDOW_WIDTH = 1000.0;
const double WINDOW_HEIGHT = 500.0;
const int star_points = 5;
const double rotation_rate = M_PI/1000;
const double initial_vx = 500.0;
const double initial_vy = 500.0;
const double initial_starx = 200.0;
const double initial_stary = 200.0;
const double initial_star_pointx = 0.0;
const double initial_star_pointy = 100.0;
const double star_point_length = 50.0;


void compute_new_position(vec_list_t *object_pos, double dt, vector_t *velocity) {
  double xvelocity = velocity->x * dt;
  double yvelocity = velocity->y * dt;
  double to_translate_x = 0.0;
  double to_translate_y = 0.0;
  polygon_rotate(object_pos, rotation_rate, polygon_centroid(object_pos));
  for (int i = 0; i < star_points * 2; i++){
    vector_t *curr = vec_list_get(object_pos, i);
    if (curr->x + xvelocity > WINDOW_WIDTH || curr->x + xvelocity < 0) {
      double check = fmod((xvelocity + curr->x), WINDOW_WIDTH);
      if (fabs(check) > fabs(to_translate_x)) {
        to_translate_x = check;
      }
    }
    if (curr->y + yvelocity > WINDOW_HEIGHT || curr->y + yvelocity < 0) {
      double check = fmod((yvelocity + curr->y), WINDOW_HEIGHT);
      if (fabs(check) > fabs(to_translate_y)) {
        to_translate_y = check;
      }
    }
  }
  vector_t alteredVelocity = {xvelocity, yvelocity};
  vector_t toBounce = {-2 *to_translate_x, -2 * to_translate_y};
  polygon_translate(object_pos, alteredVelocity);
  polygon_translate(object_pos, toBounce);
  if (to_translate_x != 0) {
    velocity->x = -velocity->x;
  }
  if (to_translate_y != 0) {
    velocity->y = -velocity->y;
  }
}

int main (int argc, char *argv[]) {
  vector_t *velocity = malloc(sizeof(vector_t));
  assert(velocity != NULL);
  velocity->x = initial_vx;
  velocity->y = initial_vy;
  vector_t topRight = {WINDOW_WIDTH, WINDOW_HEIGHT};
  vector_t bottomLeft = {0.0, 0.0};
  sdl_init(bottomLeft, topRight);
  vec_list_t *object_pos = vec_list_init(10);
  double dt = 0.0;
  for (int i = 0; i < 10; i++) {
    vector_t *new = malloc(sizeof(vector_t));
    assert(new != NULL);
    new->x = initial_star_pointx;
    new->y = initial_star_pointy;
    if ((i % 2) == 0){
      new->y = star_point_length;
    }
    double newx = new->x * cos(M_PI/star_points * i) - new->y * sin(M_PI/star_points * i);
    double newy = new->x * sin(M_PI/star_points * i) + new->y * cos(M_PI/star_points * i);
    new->x = newx;
    new->y = newy;
    vec_list_add(object_pos, new);
  }
  vector_t toTranslate = {initial_starx, initial_starx};
  polygon_translate(object_pos, toTranslate);
  while (!sdl_is_done()) {
    dt = time_since_last_tick();
    compute_new_position(object_pos, dt, velocity);
    sdl_clear();
    sdl_draw_polygon(object_pos, 0, 1, 0);
    sdl_show();
  }
  free(velocity);
  vec_list_free(object_pos);
}

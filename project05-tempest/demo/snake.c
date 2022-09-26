#include "sdl_wrapper.h"
#include <math.h>
#include <stdlib.h>
#include "forces.h"
#include <assert.h>

const double K = 40;
const int INITIAL_PELLETS = 30;
const size_t PELLET_POINTS = 80;
const double GAMMA = 0.1;
const double K_DECREASE = .4;
const double GAMMA_DECREASE = .01;
const int CENTER_INCREASE = 40;
const double RADIUS_SCALAR = 2.0;
const double PELLET_MASS = 1.0;


rgb_color_t random_color(){
  rgb_color_t color;
  color.r = ((float) rand() / ((float) RAND_MAX));
  color.b = ((float) rand() / ((float) RAND_MAX));
  color.g = ((float) rand() / ((float) RAND_MAX));
  return color;
}

body_t *make_pellet(double cx, double cy, double radius, double mass, rgb_color_t color) {
  list_t *c = list_init(PELLET_POINTS, free);
  for (int i = 0; i < PELLET_POINTS; i++) {
      double angle = 2 * M_PI * i /  PELLET_POINTS;
      vector_t *v = malloc(sizeof(vector_t));
      assert(v != NULL);
      *v = (vector_t){cx + radius * cos(angle), cy + radius * sin(angle)};
      list_add(c, v);
    }
    body_t *pellet = body_init(c, mass, color);
    return pellet;
}

int main(){
  vector_t min = {0, 0};
  vector_t max = {WINDOW_WIDTH, WINDOW_HEIGHT};
  sdl_init(min, max);
  scene_t *scene = scene_init();
  rgb_color_t white = {1, 1, 1};
  for (int i = 0; i < INITIAL_PELLETS; i++){
    double radius = ((double) WINDOW_WIDTH) / ((double) INITIAL_PELLETS) / RADIUS_SCALAR;
    double x = ((double) i) * ((double) WINDOW_WIDTH) / ((double) INITIAL_PELLETS) + radius;
    double y = ((double) WINDOW_HEIGHT / RADIUS_SCALAR) + CENTER_INCREASE;
    scene_add_body(scene, make_pellet(x, ((double) WINDOW_HEIGHT) / RADIUS_SCALAR, PELLET_MASS, radius, white));
    scene_add_body(scene, make_pellet(x, y, radius, PELLET_MASS, random_color()));
    create_spring(scene, K - (K_DECREASE * i), scene_get_body(scene, 2 * i), scene_get_body(scene, 2 * i + 1));
    create_drag(scene, GAMMA + (GAMMA_DECREASE * i), scene_get_body(scene, 2 * i + 1));
  }
  while (sdl_is_done(scene) != true){
    double dt = time_since_last_tick();
    scene_tick(scene, dt);
    sdl_render_scene(scene);
  }
  scene_free(scene);
}

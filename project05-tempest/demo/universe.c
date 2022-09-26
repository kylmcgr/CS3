#include "sdl_wrapper.h"
#include <math.h>
#include <stdlib.h>
#include "forces.h"

const int STAR_POINTS = 4;
const float MASS_UPPER = 30.0F;
const float MASS_LOWER = 10.0F;
const int NUMBER_OF_STARS = 50;
const double G = 1000.0;
const double OUTER_STAR_CONST = .4;
const double INNER_STAR_CONST = 2.5;


rgb_color_t random_color(){
  rgb_color_t color;
  color.r = ((float) rand() / ((float) RAND_MAX));
  color.b = ((float) rand() / ((float) RAND_MAX));
  color.g = ((float) rand() / ((float) RAND_MAX));
  return color;
}

body_t *make_star(){
  size_t list_size = STAR_POINTS * 2;
  list_t *points = list_init(list_size, free);
  double mass = ((float) rand() / ((float) RAND_MAX)) * (MASS_UPPER - MASS_LOWER) + (MASS_LOWER);
  double radius = mass;
  vector_t initial = {0, radius};

  //finding angle between points
  double theta = (2 * M_PI) / list_size;
  for (size_t i = 0; i < list_size / 2; i++){
    vector_t *new_out = malloc(sizeof(vector_t));
    vector_t *new_in = malloc(sizeof(vector_t));
    initial = vec_rotate(initial, theta);
    *new_out = initial;

    //adding outer point
    list_add(points, new_out);
    initial = vec_multiply(OUTER_STAR_CONST, initial);
    initial = vec_rotate(initial, theta);
    *new_in = initial;

    //adding inner point
    list_add(points, new_in);
    initial = vec_multiply(INNER_STAR_CONST, initial);
  }
  double randx = (double)(rand() % (WINDOW_WIDTH - 0 + 1)) + 0;
  double randy = (double)(rand() % (WINDOW_HEIGHT - 0 + 1)) + 0;
  vector_t random_center = {randx, randy};
  body_t *star = body_init(points, mass, random_color());
  body_set_centroid(star, random_center);
  return star;
}

int main(){
  vector_t min = {0, 0};
  vector_t max = {WINDOW_WIDTH, WINDOW_HEIGHT};
  sdl_init(min, max);
  scene_t *scene = scene_init();
  for (int i = 0; i < NUMBER_OF_STARS; i++){
    scene_add_body(scene, make_star());
    for (int j = 0; j < i; j++){
      create_newtonian_gravity(scene, G, scene_get_body(scene, i), scene_get_body(scene, j));
    }
  }
  while (sdl_is_done(scene) != true){
    double dt = time_since_last_tick();
    scene_tick(scene, dt);
    sdl_render_scene(scene);
  }
  scene_free(scene);
}

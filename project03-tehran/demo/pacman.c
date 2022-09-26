#include <stdlib.h>
#include <math.h>
#include "sdl_wrapper.h"

const size_t pellet_points = 10;
const size_t pacman_points = 400;
const int initial_pellets = 30;
const double base_speed = 200.0;
const double acceleration = 700.0;
const double pellet_size = 5;
const double pacman_size = 50;
const double start_angle = M_PI * .225;
const double end_angle = M_PI *1.775;
const int color_upper = 1;
const int color_lower = 0;
const rgb_color_t pacman_color = {.996, .41, .703};


rgb_color_t random_color(){
  rgb_color_t color;
  color.r = (float)(rand() % (color_upper - color_lower + 1)) + color_lower;
  color.b = (float)(rand() % (color_upper - color_lower + 1)) + color_lower;
  color.g = (float)(rand() % (color_upper - color_lower + 1)) + color_lower;
  return color;
}

body_t *make_pellet() {
  list_t *c = list_init(pellet_points, free);
  double cx = rand() % WINDOW_WIDTH;
  double cy = rand() % WINDOW_HEIGHT;
  for (int i = 0; i < pellet_points; i++) {
      double angle = 2 * M_PI * i / pellet_points;
      vector_t *v = malloc(sizeof(vector_t));
      *v = (vector_t) {cx + pellet_size*cos(angle), cy + pellet_size*sin(angle)};
      list_add(c, v);
    }
    body_t *pellet = body_init(c, 1, random_color());
    return pellet;
}

body_t *make_pacman(){
  list_t *c = list_init(pacman_points, free);
  double cx = WINDOW_WIDTH / 2;
  double cy = WINDOW_HEIGHT / 2;
  for (size_t i = 0; i < pacman_points; i++) {
      double angle = start_angle + 2 * M_PI * i / pacman_points;
      if (angle >= end_angle){
        break;
      }
      vector_t *v = malloc(sizeof(vector_t));
      *v = (vector_t) {cx + pacman_size*cos(angle), cy + pacman_size*sin(angle)};
      list_add(c, v);
  }
  // compute and set location of pacman
  vector_t *center = malloc(sizeof(vector_t));
  center->x = cx;
  center->y = cy;
  list_add(c, center);
  body_t *pacman = body_init(c, 1, pacman_color);
  return pacman;
}

void handler(scene_t *scene, char key, key_event_type_t type, double held_time){
  body_t *pacman = scene_get_body(scene, 0);
  vector_t velocity = body_get_velocity(pacman);
  double new_velocity = base_speed;
  vector_t new_v = {0.0, 0.0};
  if (type == KEY_PRESSED){
    new_velocity += held_time * acceleration;
  }
  if (key == UP_ARROW){
    new_v.x = 0;
    new_v.y = new_velocity;
  }
  if (key == DOWN_ARROW){
    new_v.x = 0;
    new_v.y = -new_velocity;
  }
  if(key == RIGHT_ARROW){
    new_v.x = new_velocity;
    new_v.y = 0;
  }
  if(key == LEFT_ARROW){
    new_v.x = -new_velocity;
    new_v.y = 0;
  }
  if (velocity.x == 0 && velocity.y == 0){
    velocity.x = base_speed;
  }
  if (velocity.x != new_v.x || velocity.y != new_v.y){
    double rotate = atan2(new_v.y, new_v.x) - atan2(velocity.y, velocity.x);
    body_set_rotation(pacman, rotate);
  }
  body_set_velocity(pacman, new_v);
}

void wrap(scene_t *scene){
  body_t *pacman = scene_get_body(scene, 0);
  vector_t vec = body_get_centroid(pacman);
  if (vec.y <= 0){
    vec.y = WINDOW_HEIGHT - 5;
  }
  if (vec.y >= WINDOW_HEIGHT){
    vec.y = 5;
  }
  if (vec.x >= WINDOW_WIDTH){
    vec.x = 5;
  }
  if (vec.x <= 0){
    vec.x = WINDOW_WIDTH - 5;
  }
  body_set_centroid(pacman, vec);
}

void remove_pellet(scene_t *scene){
  body_t *pacman = scene_get_body(scene, 0);
  vector_t pacman_center = body_get_centroid(pacman);
  for (int i = 1; i < scene_bodies(scene); i++){
    body_t *pellet_i = scene_get_body(scene, i);
    vector_t pellet_center_i = body_get_centroid(pellet_i);
    vector_t dif = vec_subtract(pacman_center, pellet_center_i);
    double vec_magnitude = sqrt(vec_dot(dif, dif));
    if (vec_magnitude < (pellet_size + pacman_size * .75)){
      scene_remove_body(scene, i);
      break;
    }
  }
}

// adds the bodies to the graphic and moves them
int main(){
  vector_t min = {0, 0};
  vector_t max = {WINDOW_WIDTH, WINDOW_HEIGHT};
  sdl_init(min, max);
  scene_t *scene = scene_init();
  sdl_on_key(handler);
  double counter_time = 0;
  scene_add_body(scene, make_pacman());
  for (int i = 0; i < initial_pellets; i++){
    scene_add_body(scene, make_pellet());
  }
  while (sdl_is_done(scene) != true){
    remove_pellet(scene);
    wrap(scene);
    double dt = time_since_last_tick();
    counter_time += dt;
    if (counter_time > .85){
      scene_add_body(scene, make_pellet());
      counter_time = 0;
    }
    scene_tick(scene, dt);
    sdl_render_scene(scene);
  }
  scene_free(scene);
}

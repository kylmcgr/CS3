#include <stdlib.h>
#include <math.h>
#include "sdl_wrapper.h"

const size_t PELLET_POINTS = 10;
const size_t PACMAN_POINTS = 400;
const int INITIAL_PELLETS = 30;
const double BASE_SPEED = 200.0;
const double ACCELERATION = 700.0;
const double PELLET_SIZE = 5;
const double PACMAN_SIZE = 50;
const double START_ANGLE = M_PI * .225;
const double END_ANGLE = M_PI *1.775;
const int COLOR_UPPER = 1;
const int COLOR_LOWER = 0;
const rgb_color_t PACMAN_COLOR = {.996F, .41F, .703F};
const int VEL_FIVE = 5;
const double VEL_SCALAR = .75;
const double TIME_MIN = 0.85;
const double WINDOW_SCALAR = 2.0;

rgb_color_t random_color(){
  rgb_color_t color;
  color.r = ((float) rand() / ((float) RAND_MAX));
  color.b = ((float) rand() / ((float) RAND_MAX));
  color.g = ((float) rand() / ((float) RAND_MAX));
  return color;
}

body_t *make_pellet() {
  list_t *c = list_init(PELLET_POINTS, free);
  double cx = rand() % WINDOW_WIDTH;
  double cy = rand() % WINDOW_HEIGHT;
  for (int i = 0; i < PELLET_POINTS; i++) {
      double angle = 2 * M_PI * i / PELLET_POINTS;
      vector_t *v = malloc(sizeof(vector_t));
      *v = (vector_t) {cx + PELLET_SIZE*cos(angle), cy + PELLET_SIZE*sin(angle)};
      list_add(c, v);
    }
    body_t *pellet = body_init(c, 1, random_color());
    return pellet;
}

body_t *make_pacman(){
  list_t *c = list_init(PACMAN_POINTS, free);
  double cx = WINDOW_WIDTH / WINDOW_SCALAR;
  double cy = WINDOW_HEIGHT / WINDOW_SCALAR;
  for (size_t i = 0; i < PACMAN_POINTS; i++) {
      double angle = START_ANGLE + 2 * M_PI * i / PACMAN_POINTS;
      if (angle >= END_ANGLE){
        break;
      }
      vector_t *v = malloc(sizeof(vector_t));
      *v = (vector_t) {cx + PACMAN_SIZE*cos(angle), cy + PACMAN_SIZE*sin(angle)};
      list_add(c, v);
  }
  // compute and set location of pacman
  vector_t *center = malloc(sizeof(vector_t));
  center->x = cx;
  center->y = cy;
  list_add(c, center);
  body_t *pacman = body_init(c, 1, PACMAN_COLOR);
  return pacman;
}

void handler(void *scene, char key, key_event_type_t type, double held_time){
  body_t *pacman = scene_get_body(scene, 0);
  vector_t velocity = body_get_velocity(pacman);
  double new_velocity = BASE_SPEED;
  vector_t new_v = {0.0, 0.0};
  if (type == KEY_PRESSED){
    new_velocity += held_time * ACCELERATION;
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
    velocity.x = BASE_SPEED;
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
    vec.y = WINDOW_HEIGHT - VEL_FIVE;
  }
  if (vec.y >= WINDOW_HEIGHT){
    vec.y = VEL_FIVE;
  }
  if (vec.x >= WINDOW_WIDTH){
    vec.x = VEL_FIVE;
  }
  if (vec.x <= 0){
    vec.x = WINDOW_WIDTH - VEL_FIVE;
  }
  body_set_centroid(pacman, vec);
}

void remove_pellet(scene_t *scene){
  body_t *pacman = scene_get_body(scene, 0);
  vector_t pacman_center = body_get_centroid(pacman);
  for (size_t i = 1; i < scene_bodies(scene); i++){
    body_t *pellet_i = scene_get_body(scene, i);
    vector_t pellet_center_i = body_get_centroid(pellet_i);
    vector_t dif = vec_subtract(pacman_center, pellet_center_i);
    double vec_magnitude = sqrt(vec_dot(dif, dif));
    if (vec_magnitude < (PELLET_SIZE + PACMAN_SIZE * VEL_SCALAR)){
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
  for (int i = 0; i < INITIAL_PELLETS; i++){
    scene_add_body(scene, make_pellet());
  }
  while (sdl_is_done(scene) != true){
    remove_pellet(scene);
    wrap(scene);
    double dt = time_since_last_tick();
    counter_time += dt;
    if (counter_time > TIME_MIN){
      scene_add_body(scene, make_pellet());
      counter_time = 0;
    }
    scene_tick(scene, dt);
    sdl_render_scene(scene);
  }
  scene_free(scene);
}

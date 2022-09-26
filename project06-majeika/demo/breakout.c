#include <stdlib.h>
#include <math.h>
#include "sdl_wrapper.h"
#include "forces.h"
#include <stdio.h>
#include <assert.h>

const double FORT_WIDTH = 20.0;
const size_t BUBBLE_POINTS = 50;
const double BUBBLE_SIZE = 5.0;
const double BUBBLE_VELOCITY = 400.0;
const int FRIENDLY = 0;
const int ENEMY = 1;
const rgb_color_t PINK_COLOR = {.996F, .41F, .703F};
const double FORT_SPEED = 500.0;
const int RECTANGLE_POINTS = 4;
const double BRICK_SPEED = 500.0;
const int ROWS = 3;
const int COLUMNS = 10;
const double ELASTICITY = 1.0;
const double SPACING = 5;
const double TIME_MIN = 5.0;
const int BUBBLE = 2;

rgb_color_t random_color(){
  rgb_color_t color;
  color.r = ((float) rand() / ((float) RAND_MAX));
  color.b = ((float) rand() / ((float) RAND_MAX));
  color.g = ((float) rand() / ((float) RAND_MAX));
  return color;
}

// makes an arbitrary sized rectangle that can be reapplied to make useful polygons
list_t *make_rectangle(double width, double length){
  list_t *rectangle = list_init(RECTANGLE_POINTS, free);
  vector_t *v = malloc(sizeof(vector_t));
  assert(v != NULL);
  *v = (vector_t) {length, width};
  list_add(rectangle, v);
  v = malloc(sizeof(vector_t));
  *v = (vector_t) {length, 0};
  list_add(rectangle, v);
  v = malloc(sizeof(vector_t));
  *v = (vector_t) {0 , 0};
  list_add(rectangle, v);
  v = malloc(sizeof(vector_t));
  *v = (vector_t) {0, width};
  list_add(rectangle, v);
  return rectangle;
}

// one body is destroyed the other bounces
void apply_better_collision(body_t *body1, body_t *body2, vector_t axis, void *aux){
  double mass1 = body_get_mass(body1);
  double *elasticity = (double *) aux;
  double velocity1axis = vec_dot(body_get_velocity(body1), axis);
  double velocity2axis = vec_dot(body_get_velocity(body2), axis);
  double impulse = mass1 * (1 + *elasticity) * (velocity2axis - velocity1axis);
  vector_t impulse_vec1 = vec_multiply(impulse, axis);
  body_add_impulse(body1, impulse_vec1);
  body_remove(body2);
}

// creates different collisions between scene bodies
void collision(scene_t *scene, body_t *body){
  if (*(int *) body_get_info(body) == BUBBLE){
    for (size_t i = 0; i < scene_bodies(scene); i++){
      body_t *curr = scene_get_body(scene, i);
      if (*(int *) body_get_info(curr) == FRIENDLY){
        create_physics_collision(scene, ELASTICITY, body, curr);
      }
    }
  }
  else {
    double *aux = malloc(sizeof(double));
    assert(aux != NULL);
    *aux = ELASTICITY;
    body_t *bubble = scene_get_body(scene, 1);
    create_collision(scene, bubble, body, (collision_handler_t) apply_better_collision,
    (void *)aux, free);
  }
}

// walls with infinite mass
void make_walls(scene_t *scene){
  double RECTANGLE_LENGTH = WINDOW_WIDTH / (COLUMNS + 1);
  int *status1 = malloc(sizeof(int));
  *status1 = FRIENDLY;
  int *status2 = malloc(sizeof(int));
  assert(status2 != NULL && status1 != NULL);
  *status2 = FRIENDLY;
  body_t *wall1 = body_init_with_info(make_rectangle(WINDOW_HEIGHT,
    RECTANGLE_LENGTH), INFINITY, PINK_COLOR, status1, free);
  body_set_centroid(wall1, (vector_t){0 - RECTANGLE_LENGTH / 2, WINDOW_HEIGHT / 2});
  body_t *wall2 = body_init_with_info(make_rectangle(WINDOW_HEIGHT,
    RECTANGLE_LENGTH), INFINITY, PINK_COLOR, status2, free);
  body_set_centroid(wall2, (vector_t){WINDOW_WIDTH + RECTANGLE_LENGTH / 2,
    WINDOW_HEIGHT / 2});
  scene_add_body(scene, wall1);
  scene_add_body(scene, wall2);
}


// creating bubble that bounces between fort and bricks
body_t *make_bubble() {
  list_t *bubble_shape = list_init(BUBBLE_POINTS, free);
  double bubble_x = WINDOW_WIDTH / 2;
  double bubble_y = FORT_WIDTH + BUBBLE_SIZE; //removed divide by 2
  for (size_t i = 0; i < BUBBLE_POINTS; i++) {
      double angle = 2 * M_PI * i / BUBBLE_POINTS;
      vector_t *v = malloc(sizeof(vector_t));
      assert(v != NULL);
      *v = (vector_t) {bubble_x + BUBBLE_SIZE * cos(angle), bubble_y +
        BUBBLE_SIZE * sin(angle)};
      list_add(bubble_shape, v);
    }
    int *status = malloc(sizeof(int));
    *status = BUBBLE;
  body_t *bubble = body_init_with_info(bubble_shape, 1, PINK_COLOR, status, free);
  body_set_velocity(bubble, (vector_t) {BUBBLE_VELOCITY, BUBBLE_VELOCITY});
  return bubble;
}

body_t *make_fort(){
  double cx = WINDOW_WIDTH / 2;
  double cy = FORT_WIDTH / 2;
  list_t *fort_shape = make_rectangle(FORT_WIDTH, 100);
  int *status = malloc(sizeof(int));
  assert(status != NULL);
  *status = FRIENDLY;
  body_t *fort = body_init_with_info(fort_shape, INFINITY, random_color(), status, free);
  body_set_centroid(fort, (vector_t) {cx, cy});
  body_set_velocity(fort, (vector_t) {0.0, 0.0});
  return fort;
}

body_t *make_single_brick(rgb_color_t color){
  double RECTANGLE_LENGTH = WINDOW_WIDTH / (COLUMNS + 1);
  list_t *shape = make_rectangle(FORT_WIDTH, RECTANGLE_LENGTH);
  int *status = malloc(sizeof(int));
  assert(status != NULL);
  *status = ENEMY;
  body_t *brick = body_init_with_info(shape, 1, color, status, free);
  return brick;
}

void make_bricks(scene_t *scene, int add) {
  double BRICK_LENGTH = WINDOW_WIDTH / (COLUMNS + 1);
  double spacing = BRICK_LENGTH / (COLUMNS - 1);
  for (int j = 0; j < COLUMNS; j++){
    double brick_x = (BRICK_LENGTH / 2) * (2 * j + 1) + (spacing * j);
    rgb_color_t color = random_color();
    if (add == 0){
      for (int i = 0; i < ROWS; i++){
        body_t *brick = make_single_brick(color);
        double brick_y = WINDOW_HEIGHT - ((FORT_WIDTH / 2) * (2 * i + 1) + (spacing * i));
        body_set_centroid(brick, (vector_t) {brick_x, brick_y});
        scene_add_body(scene, brick);
        collision(scene, brick);
      }
    }
    else if(add != 0){
      int curr_row = ROWS + add - 1;
      body_t *brick = make_single_brick(color);
      double brick_y = WINDOW_HEIGHT - ((FORT_WIDTH / 2) * (2 * curr_row + 1) + (spacing * curr_row));
      body_set_centroid(brick, (vector_t) {brick_x, brick_y});
      scene_add_body(scene, brick);
      collision(scene, brick);
    }
  }
}

// wraps the fort around the screen
void wrap(scene_t *scene){
  body_t *fort = scene_get_body(scene, 0);
  vector_t vec = body_get_centroid(fort);
  if (vec.x >= WINDOW_WIDTH){
    vec.x = SPACING;
  }
  if (vec.x <= 0){
    vec.x = WINDOW_WIDTH - SPACING;
  }
  body_set_centroid(fort, vec);
}

void handler(void *scene, char key, key_event_type_t type, double held_time){
  body_t *fort = scene_get_body(scene, 0);
  if (*(int *)body_get_info(fort) == ENEMY){
    return;
  }
  vector_t new_v = (vector_t) {0.0, 0.0};
  if (type == KEY_RELEASED){
    body_set_velocity(fort, new_v);
  }
  else if (key == RIGHT_ARROW){
    new_v.x = FORT_SPEED;
  }
  else if (key == LEFT_ARROW){
    new_v.x = -FORT_SPEED;
  }
  body_set_velocity(fort, new_v);
}

// checks if the bubble hits the bottom of the screen (causing a reset)
bool check_bubble(scene_t *scene){
  body_t *bubble = scene_get_body(scene, 1);
  vector_t curr_centroid = body_get_centroid(bubble);
  if (curr_centroid.y <= 0){
    return true;
  }
  return false;
}

void start_game(scene_t *scene){
  body_t *fort = make_fort();
  scene_add_body(scene, fort);
  body_t *bubble = make_bubble();
  scene_add_body(scene, bubble);
  make_walls(scene);
  make_bricks(scene, 0);
  collision(scene, bubble);
}

int main(){
  vector_t min = {0, 0};
  vector_t max = {WINDOW_WIDTH, WINDOW_HEIGHT};
  sdl_init(min, max);
  scene_t *scene = scene_init();
  sdl_on_key(handler);
  double counter_time = 0;
  start_game(scene);
  int add = 1;
  while (sdl_is_done(scene) != true){
    wrap(scene);
    double dt = time_since_last_tick();
    counter_time += dt;
    if(check_bubble(scene)){
      scene_free(scene);
      scene_t *new_scene = scene_init();
      scene = new_scene;
      start_game(scene);
      add = 1;
      counter_time = 0;
    }
    if (counter_time > TIME_MIN){
      make_bricks(scene, add);
      add++;
      counter_time = 0;
    }
    scene_tick(scene, dt);
    sdl_render_scene(scene);
  }
  scene_free(scene);
}

#include <stdlib.h>
#include <math.h>
#include "sdl_wrapper.h"
#include "forces.h"
#include <stdio.h>

const size_t ALIEN_POINTS = 100;
const double ALIEN_SIZE = 1000/22;
const double ALIEN_SPACING = 2.0;
const double ALIEN_VELOCITY = 200.0;
const double START_ALIEN_ANGLE = M_PI / 6;
const double END_ALIEN_ANGLE = 5 * M_PI / 6;
const rgb_color_t ALIEN_COLOR = {.996F, .41F, .703F};
const size_t NUM_ALIENS = 21;
const size_t FORT_POINTS = 50;
const double FORT_SPEED = 1000.0;
const double FORT_SIZE = 20.0;
const rgb_color_t FORT_COLOR = {.82F, .82F, .82F};
const int NUKE_POINTS = 4;
const double NUKE_LENGTH = 20.0;
const double NUKE_WIDTH = 10.0;
const double NUKE_SPEED = 5000.0;
const double TIME_MIN = 0.2;
const int WINDOW_SCALAR = 2;
const int FRIENDLY = 0;
const int ENEMY = 1;
const int ALIEN_NUKE = 2;
const int VAL_FIVE = 5;

rgb_color_t random_color(){
  rgb_color_t color;
  color.r = ((float) rand() / ((float) RAND_MAX));
  color.b = ((float) rand() / ((float) RAND_MAX));
  color.g = ((float) rand() / ((float) RAND_MAX));
  return color;
}

// creating nuke that shoots from fort or from alien
body_t *make_nuke(body_t *body) {
  list_t *nuke_shape = list_init(NUKE_POINTS, free);
  double nuke_x = body_get_centroid(body).x;
  double nuke_y = body_get_centroid(body).y;
  vector_t *v = malloc(sizeof(vector_t));
  *v = (vector_t) {nuke_x + NUKE_WIDTH / 2 , nuke_y + NUKE_LENGTH / 2};
  list_add(nuke_shape, v);
  v = malloc(sizeof(vector_t));
  *v = (vector_t) {nuke_x + NUKE_WIDTH / 2 , nuke_y - NUKE_LENGTH / 2};
  list_add(nuke_shape, v);
  v = malloc(sizeof(vector_t));
  *v = (vector_t) {nuke_x - NUKE_WIDTH / 2 , nuke_y - NUKE_LENGTH / 2};
  list_add(nuke_shape, v);
  v = malloc(sizeof(vector_t));
  *v = (vector_t) {nuke_x - NUKE_WIDTH / 2 , nuke_y + NUKE_LENGTH / 2};
  list_add(nuke_shape, v);
  int *status = malloc(sizeof(int));
  *status = FRIENDLY;
  vector_t velocity = (vector_t) {0, NUKE_SPEED};
  if (*(int *)body_get_info(body) != FRIENDLY) {
    *status = ALIEN_NUKE;
    velocity.y = -velocity.y;
  }
  body_t *nuke = body_init_with_info(nuke_shape, 1, random_color(), status, free);
  body_set_centroid(nuke, (vector_t) {nuke_x, nuke_y});
  body_set_velocity(nuke, velocity);
  return nuke;
}

// randomly generates alien from which the nuke is shot
body_t *random_alien(scene_t *scene){
  size_t random_idx = (rand() % (NUM_ALIENS + 1)) + 1;
  return scene_get_body(scene, random_idx);
}

// creating individual aliens
body_t *make_alien(){
  list_t *c = list_init(ALIEN_POINTS, free);
  double cx = WINDOW_WIDTH / WINDOW_SCALAR;
  double cy = WINDOW_WIDTH / WINDOW_SCALAR;
  for (size_t i = 0; i < ALIEN_POINTS; i++) {
      double angle = START_ALIEN_ANGLE + 2 * M_PI * i / ALIEN_POINTS;
      if (angle >= END_ALIEN_ANGLE){
        break;
      }
      vector_t *v = malloc(sizeof(vector_t));
      *v = (vector_t) {cx + ALIEN_SIZE*cos(angle), cy + ALIEN_SIZE*sin(angle)};
      list_add(c, v);
  }
  vector_t *center = malloc(sizeof(vector_t));
  center->x = cx;
  center->y = cy;
  list_add(c, center);
  int *status = malloc(sizeof(int));
  *status = ENEMY;
  body_t *alien = body_init_with_info(c, 1, ALIEN_COLOR, status, free);
  return alien;
}

// starting position of aliens in three rows
void setup_aliens(scene_t *scene){
  for (int j = 0; j < 3; j++){
    double centery = WINDOW_HEIGHT - (ALIEN_SIZE * (1.3* j + 1));
    for (size_t i = 0; i < NUM_ALIENS / 3; i++){
      body_t *alien = make_alien();
      double centerx = ALIEN_SPACING + ALIEN_SIZE * (2 * i + 1);
      body_set_centroid(alien, (vector_t) {centerx, centery});
      vector_t velocity = (vector_t) {ALIEN_VELOCITY, 0};
      body_set_velocity(alien, velocity);
      scene_add_body(scene, alien);
    }
  }
}

void move_alien(scene_t *scene){
  for(size_t i = 0; i < scene_bodies(scene); i++){
    body_t *body = scene_get_body(scene, i);
    if (*(int *) body_get_info(body) == ENEMY){
      vector_t curr_loc = body_get_centroid(body);
      vector_t new_centroid = (vector_t) {curr_loc.x, curr_loc.y};
      if (curr_loc.x + ALIEN_SIZE >= WINDOW_WIDTH){
        new_centroid.x = curr_loc.x - VAL_FIVE;
        new_centroid.y = curr_loc.y - ((.7 * ALIEN_SIZE) - ALIEN_SPACING) * 6;
        vector_t neg_velocity = vec_negate(body_get_velocity(body));
        body_set_velocity(body, neg_velocity);
      }
      if(curr_loc.x - ALIEN_SIZE <= 0){
        new_centroid.x = curr_loc.x + VAL_FIVE;
        new_centroid.y = curr_loc.y - ((.7 * ALIEN_SIZE) - ALIEN_SPACING) * 6;
        vector_t neg_velocity = vec_negate(body_get_velocity(body));
        body_set_velocity(body, neg_velocity);
      }
      body_set_centroid(body, new_centroid);
    }
  }
}


//creating the fort from which the client generates nukes
body_t *make_fort(){
  list_t *c = list_init(FORT_POINTS, free);
  double cx = WINDOW_WIDTH / 2;
  double cy = FORT_SIZE;
  for (size_t i = 0; i < FORT_POINTS; i++) {
      double angle = 2 * M_PI * i / FORT_POINTS;
      vector_t *v = malloc(sizeof(vector_t));
      *v = (vector_t) {cx + FORT_SIZE*cos(angle), cy + FORT_SIZE*sin(angle)};
      list_add(c, v);
    }
    int *status = malloc(sizeof(int));
    *status = FRIENDLY;
  body_t *fort = body_init_with_info(c, 1, FORT_COLOR, status, free);
  return fort;
}

void collision(scene_t *scene, body_t *nuke){
  if (*(int *)body_get_info(nuke) == FRIENDLY){
    for (size_t i = 1; i < scene_bodies(scene); i++){
      body_t *curr = scene_get_body(scene, i);
      if (*(int *) body_get_info(curr) == ENEMY){
        create_destructive_collision(scene, nuke, curr);
      }
    }
  }
  if (*(int *) body_get_info(nuke) == ALIEN_NUKE){
    body_t *fort = scene_get_body(scene, 0);
    create_destructive_collision(scene, nuke, fort);
  }
}

void wrap(scene_t *scene){
  body_t *fort = scene_get_body(scene, 0);
  vector_t vec = body_get_centroid(fort);
  if (vec.x >= WINDOW_WIDTH){
    vec.x = VAL_FIVE;
  }
  if (vec.x <= 0){
    vec.x = WINDOW_WIDTH - VAL_FIVE;
  }
  body_set_centroid(fort, vec);
}

void handler(void *scene, char key, key_event_type_t type, double held_time){
  body_t *fort = scene_get_body(scene, 0);
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
  else if (key == ' '){
    body_t *new_nuke = make_nuke(fort);
    scene_add_body(scene, new_nuke);
    size_t curr_idx = scene_bodies(scene);
    collision(scene, scene_get_body(scene, curr_idx - 1));
  }
  body_set_velocity(fort, new_v);
}

int main(){
  vector_t min = {0, 0};
  vector_t max = {WINDOW_WIDTH, WINDOW_HEIGHT};
  sdl_init(min, max);
  scene_t *scene = scene_init();
  sdl_on_key(handler);
  double counter_time = 0;
  body_t *fort = make_fort();
  scene_add_body(scene, fort);
  setup_aliens(scene);
  while (sdl_is_done(scene) != true){
    wrap(scene);
    double dt = time_since_last_tick();
    counter_time += dt;
    if (counter_time > TIME_MIN){
      body_t *random_alien_body = random_alien(scene);
      scene_add_body(scene, make_nuke(random_alien_body));
      size_t curr_idx = scene_bodies(scene);
      collision(scene, scene_get_body(scene, curr_idx - 1));
      counter_time = 0;
    }
    move_alien(scene);
    scene_tick(scene, dt);
    sdl_render_scene(scene);
  }
  scene_free(scene);
}

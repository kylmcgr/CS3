#include <stdlib.h>
#include <math.h>
#include "forces.h"
#include "sdl_wrapper.h"
#include <stdio.h>
#include <assert.h>

// graphics and characters
const int CHAR_POINTS = 100;
const size_t CHAR_SIZE = 50.0;
const double CHAR_SPEED = 1000.0;
const size_t POWERUP_SIZE = 30.0;
const int POWERUP_TIME_MIN = 6;
const float DEMENTOR_SIZE_LOWER = 80.0;
const double DEMENTOR_VELOCITY = 50.0;
const int DEMENTOR_TIME_MIN = 3;
const size_t PATRONUS_SIZE = 15.0;
const int PATRONUS_SPEED = 600;
const double PATRONUS_TIME_MIN = 0.2;
const rgb_color_t NO_COLOR = {0, 0, 0};
const rgb_color_t PINK_COLOR = {.996F, .41F, .703F};
const rgb_color_t BLUE_COLOR = {.678F, .847F, .902F};
const rgb_color_t GREY_COLOR = {.9F, .9F, .9F};
//shapes
const int CIRCLE_POINTS = 100;
const int TRIANGLE_POINTS = 3;
const int RECTANGLE_POINTS = 4;
const int BOX_WIDTH = 40;
const int BOX_LENGTH = 100;
const int OPTION_HEIGHT = 80;
const int OPTION_WIDTH = 140;
// graphics types
const int CHARACTER_TYPE = 0;
const int DEMENTOR = 1;
const int PATRONUS = 2;
const int POWERUP = 3;
const int BACKGROUND = 4;
const int TO_START = 5;
const int TEMP_CHAR = 6;
// powerup types
const int INVISIBILITY = 0;
const int SPLIT_SHOT = 1;
const int SLOW_DEMENTORS = 2;
const int DOUBLE_PATRONUS = 3;
const int NONE = 4;
// character types
const int HARRY = 9;
const int HERMIONE = 8;
const int RON = 7;
const int COINS = 10;
const int UPGRADE = 11;
const int START = 12;
// other constants
const int SPACING = 100;
const int GRAVITY = -2000;
const int DEM_HEALTH = 0;
const int DEM_POWER_INCREASE = 3;
const int VAL_FIVE = 5;
const double DURATION = 5.0;
const int ELASTICITY = 1;
Mix_Chunk *effects = NULL;

typedef struct game_info {
  int max_dementors;
  int curr_dementors;
  int curr_level;
  int power;
  bool in_game;
  double patronus_counter;
  double dementor_counter;
  double powerup_counter;
  int powerup_type;
  int character;
  int coins;
  int upgrade_cost;
} game_info_t;


//------------------SHAPES--------------------

// makes an arbitrary circle
list_t *make_circle(size_t size){
  list_t *c = list_init(CIRCLE_POINTS, free);
  double cx = WINDOW_WIDTH / 2;
  double cy = CHAR_SIZE;
  for (size_t i = 0; i < CIRCLE_POINTS; i++) {
    double angle = 2 * M_PI * i / CIRCLE_POINTS;
    vector_t *v = malloc(sizeof(vector_t));
    *v = (vector_t) {cx + size*cos(angle), cy + size*sin(angle)};
    list_add(c, v);
  }
  return c;
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

// returns the copy of a body's shape
list_t *body_shape_copy(list_t *list){
  list_t *copy = list_init(list_size(list), free);
  for (size_t i = 0; i < list_size(list); i++){
    vector_t *element = list_get(list, i);
    vector_t *copy_elem = malloc(sizeof(vector_t));
    copy_elem->x = element->x;
    copy_elem->y = element->y;
    list_add(copy, copy_elem);
  }
  return copy;
}

//------------------CHARACTER--------------------

//creating the character from which the client generates the patronus
body_t *make_character(rgb_color_t color, game_info_t *info){
  list_t *char_list = make_circle(CHAR_SIZE);
  int *status = malloc(sizeof(int));
  *status = CHARACTER_TYPE;
  body_t *character = body_init_with_info(char_list, 1, color, status, free);
  body_set_aux(character, NONE);
  SDL_Texture *texture = NULL;
  if (info->character == HARRY){
    texture = loadTexture("images/harry.png");
  }
  if (info->character == HERMIONE){
    texture = loadTexture("images/hermione.png");
  }
  if (info->character == RON){
    texture = loadTexture("images/ron.png");
  }
  body_set_texture(character, texture);
  return character;
}

//------------------PHYSICS COMPONENTS OF BLAST--------------------

// if the dementor hits the character, both bodies are destroyed
void apply_better_collision(body_t *body1, body_t *body2, void *aux){
  game_info_t *info = (game_info_t *) aux;
  if (info->powerup_type != INVISIBILITY){
    body_remove(body1);
    body_remove(body2);
  }
}

//removes the patronus, checks health of dementor
void apply_patronus_dementor_collision(body_t *dementor, body_t *patronus, void *aux){
  Mix_PlayChannel( -1, effects, 0 );
  game_info_t *info = (game_info_t *) aux;
  int power = info->power;
  int health = body_get_aux(dementor);
  body_remove(patronus);
  health = health - power;
  char array[100];
  sprintf(array, "%d", health);
  SDL_Texture *text = loadFromRenderedText(array, (SDL_Color) {250,250,250});
  body_set_text(dementor, text);
  body_set_aux(dementor, health);
  if (health <= 0){
    body_remove(dementor);
    info->coins += info->curr_level;
  }
}

//adds collision for invisibility powerup -- makes new temp character
void apply_invisibility(body_t *character, body_t *powerup, void *aux){
  body_set_aux(character, INVISIBILITY);
  SDL_Texture *texture = loadTexture("images/cloak.png");
  body_set_texture(character, texture);
  body_remove(powerup);
}

//adds collision for split shot powerup
void apply_split_shot(body_t *character, body_t *powerup, void *aux){
  body_set_aux(character, SPLIT_SHOT);
  body_remove(powerup);
}

//adds collision for slow dementors powerup
void apply_slow_dementors(body_t *character, body_t *powerup, void *aux){
  body_remove(powerup);
  body_set_aux(character, SLOW_DEMENTORS);
  scene_t *scene = (scene_t *) aux;
  for (size_t i = 1; i < scene_bodies(scene); i++){
    body_t *bodyi = scene_get_body(scene, i);
    if (*(int *) body_get_info(bodyi) == DEMENTOR){
      // vector_t vel = body_get_velocity(bodyi);
      // vel.x = 0.5 * x;
      vector_t new_velocity = vec_multiply(.5, body_get_velocity(bodyi));
      body_set_velocity(bodyi, new_velocity);
    }
  }
}

//adds collision for fast patronus powerup - currently not used
void apply_double_patronus(body_t *character, body_t *powerup, void *aux){
  body_remove(powerup);
  body_set_aux(character, DOUBLE_PATRONUS);
}

//adds collisions for either patronus or dementor
void patronus_dementor_collision(scene_t *scene, int type, body_t *body, game_info_t *info){
  // int *aux = malloc(sizeof(game));
  // assert(aux != NULL);
  // *aux = power;
  for(size_t i = 0; i < scene_bodies(scene); i++){
    body_t *curr = scene_get_body(scene, i);
    if(*(int *) body_get_info(curr) == type){
      if (*(int *) body_get_info(curr) == PATRONUS){
        create_collision(scene, body, curr, (collision_handler_t)
        apply_patronus_dementor_collision, (void *)info, NULL);
      }
      else{
        create_collision(scene, curr, body, (collision_handler_t)
        apply_patronus_dementor_collision, (void *)info, NULL);
      }
    }
  }
}

// creates different collisions between scene bodies
void collision(scene_t *scene, body_t *body, game_info_t *info){
  body_t *character = scene_get_body(scene, 0);
  if (*(int *) body_get_info(body) == DEMENTOR){
    create_collision(scene, body, character, (collision_handler_t)
    apply_better_collision, (void *)info, NULL);
    patronus_dementor_collision(scene, PATRONUS, body, info);
  }
  else if (*(int *) body_get_info(body) == PATRONUS){
    patronus_dementor_collision(scene, DEMENTOR, body, info);
  }
  else if (*(int *) body_get_info(body) == POWERUP){
    int type = body_get_aux(body);
    if (type == INVISIBILITY){
      create_collision(scene, character, body, (collision_handler_t) apply_invisibility,
      (void *) scene, NULL);
    }
    else if (type == SPLIT_SHOT){
      create_collision(scene, character, body, (collision_handler_t) apply_split_shot,
      (void *) scene, NULL);
    }
    else if (type == SLOW_DEMENTORS){
      create_collision(scene, character, body, (collision_handler_t) apply_slow_dementors,
      (void *) scene, NULL);
    }
    else if(type == DOUBLE_PATRONUS){
      create_collision(scene, character, body, (collision_handler_t) apply_double_patronus,
      (void *) scene, NULL);
    }
  }
}

// calculates the velocity at a boundary and adds gravity
void gravity(scene_t *scene, double dt, double gravity){
  for (size_t i = 0; i < scene_bodies(scene); i++){
    body_t *curr = scene_get_body(scene, i);
    if (*(int *) body_get_info(curr) == DEMENTOR ||
    *(int *) body_get_info(curr) == POWERUP){
      bool check_y = false;
      bool check_x = false;
      list_t *shape = body_get_shape(curr);
      vector_t vel = body_get_velocity(curr);
      for (int i = 0; i < list_size(shape); i++){
        vector_t curr = *(vector_t*)list_get(shape, i);
        if (vel.y < 0){
          if (curr.y < 0){
            check_y = true;
          }
        }
        if((vel.x < 0 && curr.x < 0) || (vel.x > 0 && curr.x > WINDOW_WIDTH)){
          check_x = true;
        }
      }
      if (check_y == true){
        vel.y = (-1) * vel.y;
      }
      vel.y = vel.y + (gravity * dt);
      if (check_x == true){
        vel.x = -vel.x;
      }
      body_set_velocity(curr, vel);
    }
  }
}

//------------------PATRONUS--------------------

// patronus that damages the health of the dementor
void *make_patronus(scene_t *scene, body_t *body, game_info_t *info) {
  list_t *patronus_shape = make_circle(PATRONUS_SIZE);
  list_t *patronus_shape2 = make_circle(PATRONUS_SIZE);
  double patronus_x = body_get_centroid(body).x;
  double patronus_y = CHAR_SIZE;
  vector_t centroid1 = (vector_t) {patronus_x, patronus_y};
  int *status = malloc(sizeof(int));
  *status = PATRONUS;
  vector_t velocity = (vector_t) {0, PATRONUS_SPEED};
  if (info->powerup_type == SPLIT_SHOT || info->powerup_type == DOUBLE_PATRONUS){
    vector_t velocity2 = (vector_t) {0, PATRONUS_SPEED};
    centroid1 = (vector_t) {patronus_x - 20, patronus_y};
    vector_t centroid2 = (vector_t) {patronus_x + 20, patronus_y};
    int *status2 = malloc(sizeof(int));
    *status2 = PATRONUS;
    body_t *patronus2 = body_init_with_info(patronus_shape2, 1, BLUE_COLOR, status2, free);
    body_set_centroid(patronus2, (vector_t) {patronus_x + 5});
    if (info->powerup_type == SPLIT_SHOT){
      velocity = (vector_t) {PATRONUS_SPEED / 2, PATRONUS_SPEED};
      velocity2 = (vector_t) {-PATRONUS_SPEED / 2, PATRONUS_SPEED};
      centroid2 = (vector_t) {patronus_x, patronus_y};
    }
    body_set_centroid(patronus2, centroid2);
    body_set_velocity(patronus2, velocity2);
    SDL_Texture *texture = loadTexture("images/patronus.png");
    body_set_texture(patronus2, texture);
    scene_add_body(scene, patronus2);
    collision(scene, patronus2, info);
  }
  body_t *patronus = body_init_with_info(patronus_shape, 1, BLUE_COLOR, status, free);
  body_set_centroid(patronus, centroid1);
  body_set_velocity(patronus, velocity);
  SDL_Texture *texture = loadTexture("images/patronus.png");
  body_set_texture(patronus, texture);
  scene_add_body(scene, patronus);
  collision(scene, patronus, info);
  return patronus;
}

//removes patronus if it leaves the screen
void remove_patronus(scene_t *scene){
  for (size_t i = 1; i < scene_bodies(scene); i++){
    body_t *bodyi = scene_get_body(scene, i);
    if (*(int *) body_get_info(bodyi) == PATRONUS){
      vector_t centroid = body_get_centroid(bodyi);
      if (centroid.y >= WINDOW_HEIGHT || centroid.x >= WINDOW_WIDTH || centroid.x <= 0){
        body_remove(bodyi);
      }
    }
  }
}

//------------------DEMENTORS--------------------

// dementor that aims to destroy the character
body_t *make_dementor(scene_t *scene, game_info_t *info){
  list_t *de = list_init(TRIANGLE_POINTS, free);
  size_t dem_size = DEMENTOR_SIZE_LOWER;
  int random_idx = rand() % 2;
  double de_y = WINDOW_HEIGHT - dem_size / 2;
  double de_x = WINDOW_WIDTH;
  for (size_t i = 0; i < TRIANGLE_POINTS; i++) {
      double angle = 2 * M_PI * i / TRIANGLE_POINTS;
      vector_t *v = malloc(sizeof(vector_t));
      *v = (vector_t) {de_x + dem_size * cos(angle), de_y + dem_size * sin(angle)};
      list_add(de, v);
  }
  int *status = malloc(sizeof(int));
  *status = DEMENTOR;
  body_t *dementor = body_init_with_info(de, 1, GREY_COLOR, status, free);
  body_set_centroid(dementor, (vector_t) {de_x, de_y});
  body_set_velocity(dementor, (vector_t) {DEMENTOR_VELOCITY, 0});
  if (info->powerup_type == SLOW_DEMENTORS){
    body_set_velocity(dementor, (vector_t) {DEMENTOR_VELOCITY * 0.5, 0});
  }
  if (random_idx == 0){
    body_set_centroid(dementor, (vector_t) {0, de_y});
  }
  scene_add_body(scene, dementor);
  collision(scene, dementor, info);
  int dementor_health_lev = 0;
  dementor_health_lev = info->curr_level * DEM_POWER_INCREASE + DEM_HEALTH;
  body_set_aux(dementor, dementor_health_lev);
  SDL_Texture *texture = loadTexture("images/dementor.png");
  body_set_texture(dementor, texture);
  char array[100];
  sprintf(array, "%d", dementor_health_lev);
  SDL_Texture *text = loadFromRenderedText(array, (SDL_Color) {250,250,250});
  body_set_text(dementor, text);
  return dementor;
}

//checks if any dementors are left in the scene
bool dementors_left(scene_t *scene, game_info_t *info){
  bool dems_left = false;
  for (size_t i = 0; i < scene_bodies(scene); i++){
    body_t *curr = scene_get_body(scene, i);
    if(*(int *) body_get_info(curr) == DEMENTOR || info->curr_dementors < info->max_dementors){
      dems_left = true;
    }
  }
  return dems_left;
}

//------------------POWER UPS--------------------

// removes powerup once the duration has passed
void remove_powerup(scene_t *scene, game_info_t *info){
  body_t *character = scene_get_body(scene, 0);
  SDL_Texture *texture = loadTexture("images/harry.png");
  if (info->character == HERMIONE){
    texture = loadTexture("images/hermione.png");
  }
  if (info->character == RON){
    texture = loadTexture("images/ron.png");
  }
  body_set_texture(character, texture);
  body_set_aux(character, NONE);
  for (size_t i = 1; i < scene_bodies(scene); i++){
    body_t *curr = scene_get_body(scene, i);
    if (info->powerup_type == SLOW_DEMENTORS){
      if (*(int *) body_get_info(curr) == DEMENTOR){
        vector_t vel = body_get_velocity(curr);
        vel = vec_multiply((1/.9), vel);
        body_set_velocity(curr, vel);
      }
    }
  }
  info->powerup_type = NONE;
}

// makes powerup of a random type
int make_powerup(scene_t *scene){
  int type = rand() % 4;
  list_t *powerup_shape = make_circle(POWERUP_SIZE);
  int random_idx = rand() % 2;
  double powerup_y = WINDOW_HEIGHT - POWERUP_SIZE;
  double powerup_x = WINDOW_WIDTH;
  int *status = malloc(sizeof(int));
  *status = POWERUP;
  body_t *powerup = body_init_with_info(powerup_shape, 1, PINK_COLOR, status, free);
  body_set_aux(powerup, type);
  body_set_centroid(powerup, (vector_t) {powerup_x, powerup_y});
  if (random_idx == 0){
    body_set_centroid(powerup, (vector_t) {0, powerup_y});
  }
  body_set_velocity(powerup, (vector_t) {DEMENTOR_VELOCITY, 0});
  SDL_Texture *texture = loadTexture("images/snitch.png");
  body_set_texture(powerup, texture);
  scene_add_body(scene, powerup);
  return type;
}

// checks for duration of powerup and tracks character movement
void during_powerup(scene_t *scene, game_info_t *info){
  double duration_counter = info->powerup_counter;
  //int type = info->powerup_type;
  if (duration_counter >= DURATION){
    remove_powerup(scene, info);
    info->powerup_counter = 0;
  }
}

//------------------LEVELS AND LOADING SCREEN--------------------

// displays the current level in top right corner
void display_level(scene_t *scene, game_info_t *info){
  body_t *level_tracker = body_init_with_info(make_rectangle(BOX_WIDTH, BOX_LENGTH),
    1, BLUE_COLOR, (void *)info, NULL);
  body_set_centroid(level_tracker, (vector_t) {WINDOW_WIDTH - 20, WINDOW_HEIGHT - 15});
  scene_add_body(scene, level_tracker);
  char str[1000] = "Level ";
  char array[1000];
  sprintf(array, "%d", info->curr_level);
  strcat(str, array);
  SDL_Texture *text = loadFromRenderedText(str, (SDL_Color) {250,250,250});
  body_set_text(level_tracker, text);
}

void display_dems_left(scene_t *scene, game_info_t *info){
  body_t *dem_tracker = body_init_with_info(make_rectangle(BOX_WIDTH, BOX_LENGTH * 2.5),
    1, BLUE_COLOR, (void *)info, NULL);
  body_set_centroid(dem_tracker, (vector_t) {55, WINDOW_HEIGHT - 15});
  scene_add_body(scene, dem_tracker);
  char str[1000] = "Dementors Left: ";
  char array[1000];
  sprintf(array, "%d", info->max_dementors - info->curr_dementors);
  strcat(str, array);
  SDL_Texture *text = loadFromRenderedText(str, (SDL_Color) {250,250,250});
  body_set_text(dem_tracker, text);
}

void display_coins(scene_t *scene, game_info_t *info){
  body_t *coins_tracker = body_init_with_info(make_rectangle(BOX_WIDTH, BOX_LENGTH * 1.25),
    1, BLUE_COLOR, (void *)info, NULL);
  body_set_centroid(coins_tracker, (vector_t) {WINDOW_WIDTH / 2 + 20, WINDOW_HEIGHT - 15});
  scene_add_body(scene, coins_tracker);
  char str[1000] = "Coins: ";
  char array[1000];
  sprintf(array, "%d", info->coins);
  strcat(str, array);
  SDL_Texture *text = loadFromRenderedText(str, (SDL_Color) {250,250,250});
  body_set_text(coins_tracker, text);
}

// adds first bodies to game
void start_level(scene_t *scene, game_info_t *info){
  body_t *character = make_character(PINK_COLOR, info);
  scene_add_body(scene, character);
  make_dementor(scene, info);
  display_level(scene, info);
}

// resets base conditions in game info
void restart_game(game_info_t *info){
  info->curr_dementors = 0;
  info->patronus_counter = 0;
  info->dementor_counter = 0;
  info->powerup_counter = 0;
  info->powerup_type = NONE;
}

// frees current scene and sets to blank scene
scene_t *make_new_scene(scene_t *scene){
  scene_free(scene);
  scene_t *new_scene = scene_init();
  return new_scene;
}

// makes the body for each character based on type
void make_loading_option(scene_t *scene, int type, char *name, int x, int y, game_info_t *info){
  int *status = malloc(sizeof(int));
  *status = type;
  int height = OPTION_HEIGHT;
  int width = OPTION_WIDTH;
  if (type == UPGRADE || type == START || type == COINS){
    width = OPTION_WIDTH * 2;
  }
  rgb_color_t option_color = NO_COLOR;
  if (info->character == type){
    option_color = GREY_COLOR;
  }
  body_t *option = body_init_with_info(make_rectangle(height, width),
    1, option_color, (void *) status, free);
  SDL_Texture *text = loadFromRenderedText(name, (SDL_Color) {250,250,250});
  body_set_text(option, text);
  body_set_centroid(option, (vector_t) {x, y});
  scene_add_body(scene, option);
}

// adds bodies to loading screen
void make_loading_screen(scene_t *scene, game_info_t *info){
  list_t *back_shape = make_rectangle(WINDOW_HEIGHT, WINDOW_WIDTH + 100); // i just added this
  body_t *background = body_init_with_info(back_shape, 1, PINK_COLOR, (void *)info, NULL);
  scene_add_body(scene, background);
  body_set_aux(background, BACKGROUND);
  SDL_Texture *texture = loadTexture("images/original.jpg");
  body_set_texture(background, texture);
  // rectangles in middle that display character options, number of coins,
  // power upgrades, and option to restart level
  char name_harry[1000] = "Harry";
  make_loading_option(scene, HARRY, name_harry, SPACING, WINDOW_HEIGHT / 2, info);
  char name_hermione[1000] = "Hermione";
  make_loading_option(scene, HERMIONE, name_hermione, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, info);
  char name_ron[1000] = "Ron";
  make_loading_option(scene, RON, name_ron, WINDOW_WIDTH - SPACING, WINDOW_HEIGHT / 2, info);
  char name_coins[1000] = "Coins: ";
  char array[1000];
  sprintf(array, "%d", info->coins);
  strcat(name_coins, array);
  make_loading_option(scene, COINS, name_coins, WINDOW_WIDTH / 2, WINDOW_HEIGHT - 2 * SPACING, info);
  char name_upgrade[1000] = "Upgrade Power: ";
  char array2[1000];
  sprintf(array2, "%d", info->upgrade_cost);
  strcat(name_upgrade, array2);
  make_loading_option(scene, UPGRADE, name_upgrade, WINDOW_WIDTH / 2, SPACING * 4, info);
  char name_start[1000] = "Start Level ";
  char array3[1000];
  sprintf(array3, "%d", info->curr_level);
  strcat(name_start, array3);
  make_loading_option(scene, START, name_start, WINDOW_WIDTH / 2, SPACING, info);
}

// checks to see if the level has been passed or failed
scene_t *passed_level(scene_t *scene, game_info_t *info){
  body_t *first_body = scene_get_body(scene, 0);
  int type = *(int *) body_get_info(first_body);
  if (type !=CHARACTER_TYPE || !dementors_left(scene, info)){
    // level over
    info->in_game = false;
    if (!dementors_left(scene, info)){
      //passed level
      info->max_dementors++;
      info->curr_level++;
      info->coins = info->coins + info->curr_level * 5;
    }
    scene = make_new_scene(scene);
    make_loading_screen(scene, info);
    restart_game(info);
  }
  return scene;
}

//------------------FUNCTIONAL COMPONENTS--------------------

// the character wraps around the screen
void wrap(scene_t *scene){
  body_t *character = scene_get_body(scene, 0);
  vector_t vec = body_get_centroid(character);
  if (vec.x >= WINDOW_WIDTH){
    vec.x = VAL_FIVE;
  }
  if (vec.x <= 0){
    vec.x = WINDOW_WIDTH - VAL_FIVE;
  }
  body_set_centroid(character, vec);
}

void handler(void *scene, char key, key_event_type_t type, double held_time){
  body_t *first_body = scene_get_body(scene, 0);
  if (*(int *)body_get_info(first_body) != CHARACTER_TYPE){
    return;
  }
  vector_t new_v = (vector_t) {0.0, 0.0};
  if (type == KEY_RELEASED){
    body_set_velocity(first_body, new_v);
  }
  else if (key == RIGHT_ARROW){
    new_v.x = CHAR_SPEED;
  }
  else if (key == LEFT_ARROW){
    new_v.x = -CHAR_SPEED;
  }
  body_set_velocity(first_body, new_v);
}

void mouse_handler2(void *scene, int mousex, int mousey){
  body_t *first_body = scene_get_body(scene, 0);
  if (body_get_aux(first_body) == BACKGROUND){
    game_info_t *info = (game_info_t *)body_get_info(first_body);
    if (mousey >= WINDOW_HEIGHT / 2 - OPTION_HEIGHT / 2 &&
        mousey <= WINDOW_HEIGHT / 2 + OPTION_HEIGHT / 2){
      if (mousex >= SPACING - OPTION_WIDTH / 2 &&
          mousex <= SPACING + OPTION_WIDTH / 2){
            info->character = HARRY;
      }
      if (mousex >= WINDOW_WIDTH / 2 - OPTION_WIDTH / 2 &&
          mousex <= WINDOW_WIDTH / 2 + OPTION_WIDTH / 2){
            info->character = HERMIONE;
      }
      if (mousex >= WINDOW_WIDTH - SPACING - OPTION_WIDTH / 2 &&
          mousex <= WINDOW_WIDTH - (SPACING - OPTION_WIDTH / 2)){
            info->character = RON;
      }
    }
    else if (mousex >= WINDOW_WIDTH / 2 - OPTION_WIDTH &&
             mousex <= WINDOW_WIDTH / 2 + OPTION_WIDTH){
      if (mousey >= ((SPACING * 4) - (OPTION_HEIGHT / 2)) &&
          mousey <= ((SPACING * 4) + (OPTION_HEIGHT / 2))){
            if (info->coins >= info->upgrade_cost){
              info->power++;
              info->coins = info->coins - info->upgrade_cost;
              info->upgrade_cost = info->upgrade_cost * 5;
            }
      }
      if (mousey >= (SPACING - (OPTION_HEIGHT)) &&
          mousey <= (SPACING + (OPTION_HEIGHT))){
            body_set_aux(first_body, TO_START);
      }
    }
  }
}

// all functions that occur during game play
scene_t *game_play(scene_t *scene, game_info_t *info, double dt){
  display_dems_left(scene, info);
  display_coins(scene, info);
  if (info->curr_dementors < info->max_dementors){
    info->dementor_counter += dt;
    info->powerup_counter += dt;
  }
  info->patronus_counter += dt;
  body_t *character = scene_get_body(scene, 0);
  if (body_get_aux(character) <= 3 && body_get_aux(character) >= 0){
    info->powerup_type = body_get_aux(character);
  }
  if(info->powerup_type != NONE){
    during_powerup(scene, info);
  }
  if (info->patronus_counter >= PATRONUS_TIME_MIN && dementors_left(scene, info)){
    make_patronus(scene, character, info);
    info->patronus_counter = 0;
  }
  if (info->dementor_counter > DEMENTOR_TIME_MIN){
    info->curr_dementors++;
    make_dementor(scene, info);
    info->dementor_counter = 0;
  }
  if (info->powerup_counter > POWERUP_TIME_MIN && body_get_aux(character)==NONE){
    make_powerup(scene);
    body_t *powerup = scene_get_body(scene, scene_bodies(scene)-1);
    collision(scene, powerup, info);
    info->powerup_counter = 0;
  }
  double gravity_value = GRAVITY;
  if (info->powerup_type == SLOW_DEMENTORS){
    gravity_value = GRAVITY / 2;
  }
  gravity(scene, dt, gravity_value);
  remove_patronus(scene);
  scene = passed_level(scene, info);
  return scene;
}

int main(){
  game_info_t *info = malloc(sizeof(game_info_t));
  info->character = HARRY;
  info->curr_level = 1;
  info->power = 1;
  info->max_dementors = 5;
  info->in_game = true;
  info->coins = 0;
  info->upgrade_cost = 10;
  restart_game(info);
  vector_t min = {0, 0};
  vector_t max = {WINDOW_WIDTH, WINDOW_HEIGHT};
  sdl_init(min, max);
  TTF_Init();
  scene_t *scene = scene_init();
  sdl_on_key(handler);
  sdl_on_mouse(mouse_handler2);
  start_level(scene, info);
  effects = loadEffects("images/wand.wav");
  SDL_Texture *background = loadTexture("images/hogwarts.jpg");
  Mix_Music *music = loadMedia("images/harry_potter.wav");
  Mix_PlayMusic(music, -1);
  while (sdl_is_done(scene) != true){
    wrap(scene);
    double dt = time_since_last_tick();
    if (info->in_game){
      scene = game_play(scene, info, dt);
    }
    else if(!info->in_game){
      make_loading_screen(scene, info);
    }
    body_t *first_body = scene_get_body(scene, 0);
    if (body_get_aux(first_body) == TO_START){
      scene = make_new_scene(scene);
      info->in_game = true;
      info->curr_dementors = 0;
      start_level(scene, info);
    }
    scene_tick(scene, dt);
    sdl_render_scene_back(scene, background);
  }
  Mix_HaltMusic();
  scene_free(scene);
}

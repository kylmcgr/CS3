#include <stdlib.h>
//#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "sdl_wrapper.h"
#include "blast_physics.h"

const rgb_color_t NO_COLOR = {0, 0, 0};
const size_t CHAR_SIZE = 50.0;
const int CHARACTER_TYPE = 0;
const int DEMENTOR = 1;
const int PATRONUS = 2;
const int POWERUP = 3;
const int INVISIBILITY = 0;
const int ELASTICITY = 1;
const int SPLIT_SHOT = 1;
const int SLOW_DEMENTORS = 2;
const int FAST_PATRONUS = 3;
const int NONE = 4;

//------------------PHYSICS COMPONENTS OF BLAST--------------------
// copies the shape of a body
list_t *body_copy(list_t *list){
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

// if the dementor hits the character, it bounces off
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

//removes the patronus, checks health of dementor
void apply_patronus_dementor_collision(body_t *dementor, body_t *patronus, void *aux){
  //power is what can keep track of the power of the patronus as it levels up
  int power = *(int *) aux;
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
  }
}

//adds collision for invisibility powerup -- makes new temp character
void apply_invisibility(body_t *character, body_t *powerup, void *aux){
  scene_t *scene = (scene_t *) aux;
  //body_set_aux(character, true);
  int *status = malloc(sizeof(int));
  *status = CHARACTER_TYPE;
  list_t *char_shape = body_get_shape(character);
  list_t *temp_shape = body_copy(char_shape);
  body_t *temp_body = body_init_with_info(temp_shape, 1, NO_COLOR, status, free);
  vector_t curr_centroid = body_get_centroid(character);
  body_set_centroid(temp_body, curr_centroid);
  scene_add_body(scene, temp_body);
  SDL_Texture *texture = loadTexture("images/cloak.jpg");
  body_set_texture(temp_body, texture);
  body_set_centroid(character, (vector_t) {WINDOW_WIDTH / 2, -CHAR_SIZE});
  body_remove(powerup);
}

//adds collision for split shot powerup
void apply_split_shot(body_t *character, body_t *powerup, void *aux){
  //body_set_aux(character, true);
  body_remove(powerup);
}

//adds collision for slow dementors powerup
void apply_slow_dementors(body_t *character, body_t *powerup, void *aux){
  body_remove(powerup);
  //body_set_aux(character, true);
  scene_t *scene = (scene_t *) aux;
  for (size_t i = 1; i < scene_bodies(scene); i++){
    body_t *bodyi = scene_get_body(scene, i);
    if (*(int *) body_get_info(bodyi) == DEMENTOR){
      vector_t new_velocity = vec_multiply(.5, body_get_velocity(bodyi));
      body_set_velocity(bodyi, new_velocity);
    }
  }
}

//adds collision for fast patronus powerup - currently not used
void apply_fast_patronus(body_t *character, body_t *powerup, void *aux){
  body_remove(powerup);
  //body_set_aux(character, true);
  scene_t *scene = (scene_t *) aux;
  for (size_t i = 1; i < scene_bodies(scene); i++){
    body_t *bodyi = scene_get_body(scene, i);
    if (*(int *) body_get_info(bodyi) == PATRONUS){
      vector_t new_velocity = vec_multiply(2, body_get_velocity(bodyi));
      body_set_velocity(bodyi, new_velocity);
    }
  }
}

//adds collisions for either patronus or dementor
void patronus_dementor_collision(scene_t *scene, int type, body_t *body, int power){
  int *aux = malloc(sizeof(double));
  assert(aux != NULL);
  *aux = power;
  for(size_t i = 0; i < scene_bodies(scene); i++){
    body_t *curr = scene_get_body(scene, i);
    if(*(int *) body_get_info(curr) == type){
      if (*(int *) body_get_info(curr) == PATRONUS){
        create_collision(scene, body, curr, (collision_handler_t) apply_patronus_dementor_collision,
        (void *)aux, NULL);
      }
      else{
        create_collision(scene, curr, body, (collision_handler_t) apply_patronus_dementor_collision,
        (void *)aux, NULL);
      }
    }
  }
}

// creates different collisions between scene bodies
void collision(scene_t *scene, body_t *body, int power){
  double *aux = malloc(sizeof(double));
  body_t *character = scene_get_body(scene, 0);
  assert(aux != NULL);
  if (*(int *) body_get_info(body) == DEMENTOR){
    *aux = ELASTICITY;
    create_collision(scene, body, character, (collision_handler_t) apply_better_collision, (void *)aux, free);
    patronus_dementor_collision(scene, PATRONUS, body, power);
  }
  else if (*(int *) body_get_info(body) == PATRONUS){
    patronus_dementor_collision(scene, DEMENTOR, body, power);
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
    else if(type == FAST_PATRONUS){//to be determined if this will be added
      create_collision(scene, character, body, (collision_handler_t) apply_fast_patronus,
      (void *) scene, NULL);
    }
  }
}

//calculates the velocity at a boundary
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

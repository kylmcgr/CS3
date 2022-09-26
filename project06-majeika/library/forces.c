#include "forces.h"
#include "collision.h"
#include "force_aux.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>


const double MIN_DISTANCE = 1;

void force_aux_free(void *aux){
  force_aux_t *force_aux = (force_aux_t *) aux;
  if(force_aux->aux_free != NULL){
    force_aux->aux_free(force_aux->aux);
  }
  if(force_aux->handler != NULL){
    free(force_aux->handler);
  }
  free(force_aux);
}

void apply_newtonian_gravity(void *aux){
  force_aux_t *force_aux = (force_aux_t *) aux;
  body_t *body1 = force_aux->body1;
  body_t *body2 = force_aux->body2;
  double G = force_aux->con;
  double mass1 = body_get_mass(body1);
  double mass2 = body_get_mass(body2);
  vector_t centroid1 = body_get_centroid(body1);
  vector_t centroid2 = body_get_centroid(body2);
  vector_t distance = vec_subtract(centroid1, centroid2);
  double dist = sqrt(vec_dot(distance, distance));
  if (dist > MIN_DISTANCE){
    vector_t unit_vec = vec_multiply(1 / dist, distance);
    vector_t gravity = vec_multiply(G * mass1 * mass2 / pow(dist, 2), unit_vec);
    body_add_force(body1, vec_negate(gravity));
    body_add_force(body2, gravity);
  }
}

void create_newtonian_gravity(scene_t *scene, double G, body_t *body1, body_t *body2){
  force_aux_t *aux = malloc(sizeof(force_aux_t));
  assert(aux != NULL);
  aux->con = G;
  aux->body1 = body1;
  aux->body2 = body2;
  aux->handler = NULL;
  aux->aux_free = NULL;
  aux->aux = NULL;
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  scene_add_bodies_force_creator(scene, (force_creator_t) apply_newtonian_gravity,
  (void*) aux, (list_t*) bodies, (free_func_t) force_aux_free);
}

void apply_spring(void *aux){
  force_aux_t *force_aux = (force_aux_t *) aux;
  body_t *body1 = force_aux->body1;
  body_t *body2 = force_aux->body2;
  double k = force_aux->con;
  vector_t center_b1 = body_get_centroid(body1);
  vector_t center_b2 = body_get_centroid(body2);
  vector_t displacement = vec_subtract(center_b1, center_b2);
  vector_t spring_force = vec_multiply(k, displacement);
  body_add_force(body1, vec_negate(spring_force));
  body_add_force(body2, spring_force);
}

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2){
  force_aux_t *aux = malloc(sizeof(force_aux_t));
  assert(aux != NULL);
  aux->con = k;
  aux->body1 = body1;
  aux->body2 = body2;
  aux->handler = NULL;
  aux->aux_free = NULL;
  aux->aux = NULL;
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  scene_add_bodies_force_creator(scene, (force_creator_t) apply_spring, (void*) aux, (list_t*) bodies,
  (free_func_t) force_aux_free);
}

void apply_drag(void *aux){
  force_aux_t *force_aux = (force_aux_t *) aux;
  double gamma = force_aux->con;
  body_t *body = force_aux->body1;
  vector_t velocity = body_get_velocity(body);
  vector_t drag_force = vec_multiply(gamma, velocity);
  drag_force = vec_negate(drag_force);
  body_add_force(body, drag_force);
}

void create_drag(scene_t *scene, double gamma, body_t *body){
  force_aux_t *aux = malloc(sizeof(force_aux_t));
  assert(aux != NULL);
  aux->con = gamma;
  aux->body1 = body;
  aux->aux = NULL;
  aux->handler = NULL;
  aux->aux_free = NULL;
  list_t *bodies = list_init(1, NULL);
  list_add(bodies, body);
  scene_add_bodies_force_creator(scene, (force_creator_t) apply_drag,
  (void*) aux,(list_t *) bodies, (free_func_t) force_aux_free);
}

void apply_collision(void *aux){
  force_aux_t *force_aux = (force_aux_t *) aux;
  collision_handler_t handler = *(collision_handler_t *) force_aux->handler;
  body_t *body1 = force_aux->body1;
  body_t *body2 = force_aux->body2;
  if (force_aux->has_collided <= 0) {
    list_t *shape1 = body_get_shape(body1);
    list_t *shape2 = body_get_shape(body2);
    void *aux1 = force_aux->aux;
    collision_info_t collision = find_collision(shape1, shape2);
    if (collision.collided){
      handler(body1, body2, collision.axis, aux1);
      force_aux->has_collided = 2;
    }
    list_free(shape1);
    list_free(shape2);
  } else {
    force_aux->has_collided--;
  }
}

void create_collision(scene_t *scene, body_t *body1, body_t *body2,
  collision_handler_t handler, void *aux, free_func_t freer){
  force_aux_t *force_aux = malloc(sizeof(force_aux_t));
  assert(force_aux != NULL);
  force_aux->body1 = body1;
  force_aux->body2 = body2;
  force_aux->aux = aux;
  collision_handler_t *force_handler = malloc(sizeof(collision_handler_t));
  assert(force_handler != NULL);
  *force_handler = handler;
  force_aux->handler = (void *) force_handler;
  force_aux->aux_free = freer;
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  scene_add_bodies_force_creator(scene, (force_creator_t) apply_collision,
  (void*) force_aux, bodies, (free_func_t) force_aux_free);
}

void apply_destructive_collision(body_t *body1, body_t *body2, vector_t axis, void *aux){
  body_remove(body1);
  body_remove(body2);
}

void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2){
  create_collision(scene, body1, body2, (collision_handler_t) apply_destructive_collision, NULL, NULL);
}

void apply_physics_collision(body_t *body1, body_t *body2, vector_t axis, void *aux){
  double mass1 = body_get_mass(body1);
  double mass2 = body_get_mass(body2);
  double velocity1axis = vec_dot(body_get_velocity(body1), axis);
  double velocity2axis = vec_dot(body_get_velocity(body2), axis);
  double *elasticity = (double *) aux;
  double masses;
  if (mass1 == INFINITY){
    masses = mass2;
  }
  else if (mass2 == INFINITY){
    masses = mass1;
  }
  else{
    masses = ((mass1 * mass2) / (mass1 + mass2));
  }
  double impulse = masses * (1 + *elasticity) * (velocity2axis - velocity1axis);
  vector_t impulse_vec1 = vec_multiply(impulse, axis);
  vector_t impulse_vec2 = vec_negate(impulse_vec1);
  body_add_impulse(body1, impulse_vec1);
  body_add_impulse(body2, impulse_vec2);
}

void create_physics_collision(scene_t *scene, double elasticity, body_t *body1, body_t *body2){
  double *aux = malloc(sizeof(double));
  assert(aux != NULL);
  *aux = elasticity;
  create_collision(scene, body1, body2, (collision_handler_t) apply_physics_collision,
  (void *)aux, free);
}

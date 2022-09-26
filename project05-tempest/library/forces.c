#include "forces.h"
#include "collision.h"
#include "force_aux.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>


const double MIN_DISTANCE = 1;


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
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  scene_add_bodies_force_creator(scene, (force_creator_t) apply_newtonian_gravity, (void*) aux, (list_t*) bodies, free);
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
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  scene_add_bodies_force_creator(scene, (force_creator_t) apply_spring, (void*) aux, (list_t*) bodies,
  free);
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
  list_t *bodies = list_init(1, NULL);
  list_add(bodies, body);
  scene_add_bodies_force_creator(scene, (force_creator_t) apply_drag, (void*) aux,(list_t *) bodies,
  free);
}

void apply_destructive_collision(void *aux){
  force_aux_t *force_aux = (force_aux_t *) aux;
  body_t *body1 = force_aux->body1;
  list_t *shape1 = body_get_shape(body1);
  body_t *body2 = force_aux->body2;
  list_t *shape2 = body_get_shape(body2);
  if (find_collision(shape1, shape2)){
    body_remove(body1);
    body_remove(body2);
  }
  list_free(shape1);
  list_free(shape2);
}

void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2){
  force_aux_t *aux = malloc(sizeof(force_aux_t));
  assert(aux != NULL);
  aux->body1 = body1;
  aux->body2 = body2;
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  scene_add_bodies_force_creator(scene, (force_creator_t) apply_destructive_collision,
  (void*) aux, bodies, free);
}

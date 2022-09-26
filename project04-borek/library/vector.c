#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


const vector_t VEC_ZERO = {0.0, 0.0};

vector_t vec_add(vector_t v1, vector_t v2){
  vector_t add_vector;
  add_vector.x = v1.x + v2.x;
  add_vector.y = v1.y + v2.y;
  return add_vector;
}

vector_t vec_subtract(vector_t v1, vector_t v2){
  vector_t subtract_vector;
  subtract_vector.x = v1.x - v2.x;
  subtract_vector.y = v1.y - v2.y;
  return subtract_vector;
}

vector_t vec_negate(vector_t v){
  vector_t inverse_vector;
  inverse_vector.x = v.x * -1;
  inverse_vector.y = v.y * -1;
  return inverse_vector;
}

vector_t vec_multiply(double scalar, vector_t v){
  vector_t scalar_vector;
  scalar_vector.x = v.x * scalar;
  scalar_vector.y = v.y * scalar;
  return scalar_vector;
}

double vec_dot(vector_t v1, vector_t v2){
  double dot_product = 0.0;
  dot_product = (v1.x * v2.x) + (v1.y * v2.y);
  return dot_product;
}

double vec_cross(vector_t v1, vector_t v2){
  double cross_product = 0.0;
  cross_product = v1.x * v2.y - v1.y * v2.x;
  return cross_product;
}

vector_t vec_rotate(vector_t v, double angle){
  vector_t vec_rotate;
  vec_rotate.x = (v.x * cos(angle)) - (v.y * sin(angle));
  vec_rotate.y = (v.x * sin(angle)) + (v.y * cos(angle));
  return vec_rotate;
}

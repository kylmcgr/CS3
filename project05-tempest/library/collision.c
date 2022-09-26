#include "collision.h"
#include "scene.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

// find min: 1, find max: -1
double min_max_dot(vector_t unit, list_t *shape, int min){
  bool var = true;
  double min_max = -1;
  for (size_t i = 0; i < list_size(shape); i++){
    double mag = vec_dot(*(vector_t *)list_get(shape, i), unit);
    if (var == true || min * mag < min * min_max){
      min_max = mag;
      var = false;
    }
  }
  return min_max;
}

void unit_vectors(list_t *units, list_t *shape){
  for (size_t i = 0; i < list_size(shape); i++) {
    vector_t p1 = *(vector_t *) list_get(shape, i);
    vector_t p2 = *(vector_t *) list_get(shape, (i + 1) % list_size(shape));
    vector_t edge = vec_subtract(p1, p2);
    double mag = 1.0/sqrt(vec_dot(edge, edge));
    vector_t unit_edge = vec_multiply(mag, edge);
    vector_t *unit = malloc(sizeof(vector_t));
    unit->x = -unit_edge.y;
    unit->y = unit_edge.x;
    list_add(units, unit);
  }
}

bool find_collision(list_t *shape1, list_t *shape2){
  list_t *units = list_init(sizeof(vector_t)*(list_size(shape1) + list_size(shape2)), free);
  unit_vectors(units, shape1);
  unit_vectors(units, shape2);
  for (size_t i = 0; i < list_size(units); i++) {
    vector_t unit = *(vector_t *) list_get(units, i);
    double min1 = min_max_dot(unit, shape1, 1);
    double max1 = min_max_dot(unit, shape1, -1);
    double min2 = min_max_dot(unit, shape2, 1);
    double max2 = min_max_dot(unit, shape2, -1);
    if (max1 < min2 || max2 < min1) {
      list_free(units);
      return false;
    }
  }
  list_free(units);
  return true;
}

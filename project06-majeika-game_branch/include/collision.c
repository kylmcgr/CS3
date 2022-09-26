#include "collision.h"
#include "scene.h"

bool find_collision(list_t *shape1, list_t *shape2){
  list_t *units = list_init((list_size(shape1) + list_size(shape2)), free);
  unit_vectors(units, shape1);
  unit_vectors(units, shape2);
  for (size_t i = 0; i < list_size(units); i++) {
    int min1 = min_max_dot(unit, shape1, 1);
    int max1 = min_max_dot(unit, shape1, -1);
    int min2 = min_max_dot(unit, shape2, 1);
    int max2 = min_max_dot(unit, shape2, -1);
    if (max1 < min2 || max2 < min1) {
      list_free(units);
      return true;
    }
  }
  list_free(units);
  return false;
}

// find min: 1, find max: -1
int min_max_dot(vector_t unit, list_t shape, int min){
  int min_max = NULL;
  for (size_t i = 0; i < list_size(shape); i++){
    vector_t dot = vec_dot(list_get(shape, i), unit);
    int mag = abs(sqrt(vec_dot(dot, dot)));
    if (min_max == NULL || min * mag < min * min_max){
      min_max = mag;
    }
  }
  return min_max;
}

void unit_vectors(list_t *units, list_t *shape){
  for (size_t i = 0; i < list_size(shape); i++) {
    vector_t unit = list_get(shape, i);
    unit = vec_subtract(unit, list_get(shape, (i + 1) % list_size(shape));
    unit = vec_multiply(sqrt(vec_dot(unit, unit)), unit);
    unit = {unit.y, -unit.x};
    list_add(units, unit);
  }
}

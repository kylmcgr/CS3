#include "vector.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct vector {
  double x;
  double y;
} vector_t;

vector_t *vec_init(double x, double y){
  vector_t *v = malloc(sizeof(vector_t));
  v->x = x;
  v->y = y;
  return v;
};

void vec_free(vector_t *vec){
  free(vec);
};

vector_t *vec_add(vector_t *v1, vector_t *v2){
  vector_t *v = malloc(sizeof(vector_t));
  v->x = v1->x + v2->x;
  v->y = v1->y + v2->y;
  return v;
};

int main(){
  vector_t *v1 = vec_init(1.0, 2.0);
  vector_t *v2 = vec_init(2.0, 3.0);
  vector_t *v = vec_add(v1, v2);
  printf("%f %f\n", v->x, v->y);
  vec_free(v1);
  vec_free(v2);
  vec_free(v);
  return 0;
}

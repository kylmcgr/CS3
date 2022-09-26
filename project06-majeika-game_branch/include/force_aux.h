#include "body.h"

typedef struct force_aux{
  double con;
  body_t *body1;
  body_t *body2;
  void *aux;
  void *handler;
  free_func_t aux_free;
  int has_collided;
} force_aux_t;

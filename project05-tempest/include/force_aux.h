#include "body.h"

typedef struct force_aux{
  double con;
  body_t *body1;
  body_t *body2;
} force_aux_t;

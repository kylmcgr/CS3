#include "sdl_wrapper.h"
#include "polygon.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>

extern const vector_t centered_loc;
extern const int color_upper;
extern const int color_lower;

typedef struct shape shape_t;

//Creates a list_t for a polygon with a set number of points
list_t* draw_polygon(size_t list_size);
shape_t *shape_init(size_t point_size, vector_t velocity, float red, float green,
  float blue);
void shape_free(shape_t *figure);
list_t *get_vectors(shape_t *figure);
vector_t get_vel(shape_t *figure);
float get_red(shape_t *figure);
float get_green(shape_t *figure);
float get_blue(shape_t *figure);
void set_vectors(shape_t *figure, list_t* vec);
void set_velocity(shape_t *figure, vector_t vel);
float color();

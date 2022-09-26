#include "sdl_wrapper.h"
#include "polygon.h"
#include "shapes.h"
#include "scene.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>

const vector_t min = {0, 0};
const vector_t max = {1000.0, 500.0};
const double rotation = .005;
const double initial_vel_x = 1000.0;
const double initial_vel_y = 1000.0;
const int list_size_num = 10;
const int out_radius = 70;
const double outer_multiplier = .4;
const double inner_multiplier = 2.5;


list_t* draw_star(size_t list_size);
vector_t  boundary(list_t *points, vector_t max, vector_t min, vector_t velocity);
rgb_color_t random_color();

rgb_color_t random_color(){
  rgb_color_t color;
  color.r = ((float) rand() / ((float) RAND_MAX));
  color.b = ((float) rand() / ((float) RAND_MAX));
  color.g = ((float) rand() / ((float) RAND_MAX));
  return color;
}

int main(){
  vector_t velocity = {initial_vel_x, initial_vel_y};
  sdl_init(min, max);
  size_t list_size = list_size_num;
  list_t *points = draw_star(list_size);
  vector_t center_poly;
  scene_t *scene = scene_init();

  // animates the graphic

  while(sdl_is_done(scene) != true){
    center_poly = polygon_centroid(points);
    double dt = time_since_last_tick();
    velocity = boundary(points, max, min, velocity);
    vector_t translation = {velocity.x * dt, velocity.y * dt};
    polygon_translate(points, translation);
    polygon_rotate(points, rotation, center_poly);
    sdl_clear();
    sdl_draw_polygon(points, random_color());
    sdl_show();
  }
  list_free(points);
  return 0;
}

//Creates a vec_list_t with the 5 inner and 5 outter points of the star
list_t* draw_star(size_t list_size){
  list_t *points = list_init(list_size, free);
  double radius_outer = out_radius;
  vector_t initial = {0, radius_outer};

  //fining angle between points
  double theta = (2 * M_PI) / list_size;
  for (size_t i = 0; i < list_size / 2; i++){
    vector_t *new_out = malloc(sizeof(vector_t));
    vector_t *new_in = malloc(sizeof(vector_t));
    initial = vec_rotate(initial, theta);
    *new_out = initial;

    //adding outer point
    list_add(points, new_out);
    initial = vec_multiply(outer_multiplier, initial);
    initial = vec_rotate(initial, theta);
    *new_in = initial;

    //adding inner point
    list_add(points, new_in);
    initial = vec_multiply(inner_multiplier, initial);
  }
  polygon_translate(points, centered_loc);
  return points;
}

// Checks the boundary conditions of the graphics. If one of the boundaries is
// hit, the velcoity is inverted.
vector_t boundary(list_t *points, vector_t max, vector_t min, vector_t velocity){
  for (size_t i = 0; i < list_size(points); i++){
    vector_t curr = *(vector_t*)list_get(points, i);

     //checks if bottom of screen is hit
     if(velocity.y < 0){
       if(curr.y < min.y){
         velocity.y = -velocity.y;
       }
     }
      //checks if top of screen is hit
      if(velocity.y > 0){
        if(curr.y > max.y){
          velocity.y = -velocity.y;
        }
      }
      //checks if left side of screen is hit
      if(velocity.x < 0){
        if(curr.x < min.x){
          velocity.x = -velocity.x;
        }
      }
      //checks if the right side of screen is hit
      if(velocity.x > 0){
        if(curr.x > max.x){
          velocity.x = -velocity.x;
        }
      }
  }
  return velocity;
}

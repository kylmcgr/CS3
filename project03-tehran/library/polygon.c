#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>
#include <assert.h>
#include "polygon.h"

double addition_cross(list_t *polygon, size_t i, size_t size);
double subtraction_cross(list_t *polygon, size_t i, size_t size);

double polygon_area(list_t *polygon){
  double cross1 = 0.0;
  size_t size = list_size(polygon);
  for (size_t i = 0; i < size; i++){
    cross1 += addition_cross(polygon, i, size);
    cross1 -= subtraction_cross(polygon, i, size);
  }
  double area = .5 * (cross1);
  return area;
}

// Computes first portion of area summation
double addition_cross(list_t *polygon, size_t i, size_t size){
  vector_t* poly = (vector_t*)list_get(polygon, i);
  vector_t* poly1 = (vector_t*)list_get(polygon, (i + 1) % size);
  double cross1 = poly->x * poly1->y;
  return cross1;

}
// Computer second portion of area summation
double subtraction_cross(list_t *polygon, size_t i, size_t size){
  vector_t* poly = (vector_t*)list_get(polygon, i);
  vector_t* poly1 = (vector_t*)list_get(polygon, (i + 1) % size);
  double cross2 = poly1->x * poly->y;
  return cross2;
}

vector_t polygon_centroid(list_t *polygon){
  size_t size = list_size(polygon);
  double sum_elem_x = 0.0;
  double sum_elem_y = 0.0;
  double cross1 = 0.0;
  double cross2 = 0.0;
  double cross3 = 0.0;
  double component_x = 0.0;
  double component_y = 0.0;
  double area = polygon_area(polygon);

  // Computes the summation equation for finding the center
  for(size_t i = 0; i < size; i++){
    vector_t* polygon_i = (vector_t*)list_get(polygon, i);
    vector_t* polygon_i1 = (vector_t*)list_get(polygon, (i + 1) % size);
    cross1 = addition_cross(polygon, i, size); // first part of summation
    cross2 = subtraction_cross(polygon, i, size); // second part of summation
    cross3 = cross1 - cross2; // final summation
    component_x = polygon_i->x + polygon_i1->x;
    component_y = polygon_i->y + polygon_i1->y;
    sum_elem_x += cross3 * component_x;
    sum_elem_y += cross3 * component_y;
  }
  sum_elem_x = (1/ (6 * area)) * sum_elem_x; // x component of center
  sum_elem_y = (1/ (6 * area)) * sum_elem_y; // y component of center
  vector_t centroid = {sum_elem_x, sum_elem_y};
  return centroid;
}

void polygon_translate(list_t *polygon, vector_t translation){
  size_t size = list_size(polygon);
  vector_t* buffer;
  for(size_t i = 0; i < size; i++){
    buffer = (vector_t*)list_get(polygon, i);
    *buffer = vec_add(*buffer, translation);
}
}

void polygon_rotate(list_t *polygon, double angle, vector_t point){
  size_t size = list_size(polygon);
  vector_t* polygon_i;
  for(size_t i = 0; i < size; i++){
    polygon_i = (vector_t*)list_get(polygon, i);
    *polygon_i = vec_subtract(*polygon_i, point);
    *polygon_i = vec_rotate(*polygon_i, angle);
    *polygon_i = vec_add(*polygon_i, point);
  }
}

#include "polygon.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/**
 * Area of polygon using shoelace formula:
 * 1/2 sum of x1*y2 + x2*y3 + ... + xn-1*yn + xn*y1 -
*             x2*y1 - x3*y2 - ... - xn*yn-1 - x1*yn
*/
double polygon_area(vec_list_t *polygon){
  double sum = 0.0;
  size_t size = vec_list_size(polygon);
  for (size_t i = 0; i < size - 1; i++){
    vector_t *curr = vec_list_get(polygon, i);
    vector_t *next = vec_list_get(polygon, i + 1);
    sum += curr->x * next->y;
    sum -= curr->y * next->x;
  }
  sum += vec_list_get(polygon, size - 1)->x * vec_list_get(polygon, 0)->y;
  sum -= vec_list_get(polygon, size - 1)->y * vec_list_get(polygon, 0)->x;
  return sum / 2;
}

/**
 * Centroid of polygon using this formula:
 * Cx = 1/(6 * area of polygon) * sum of (xi + xi+1)(xi*yi+1 - xi+1*yi)
 * Cy = 1/(6 * area of polygon) * sum of (yi + yi+1)(xi*yi+1 - xi+1*yi)
*/
vector_t polygon_centroid(vec_list_t *polygon){
  vector_t centroid = {0.0, 0.0};
  double sumX = 0.0;
  double sumY = 0.0;
  size_t size = vec_list_size(polygon);
  for(size_t i = 0; i < size; i++){
    vector_t *curr = vec_list_get(polygon, i);
    vector_t *next = vec_list_get(polygon, (i + 1) % size);
    sumX += (curr->x + next->x)* (curr->x * next->y - curr->y * next->x);
    sumY += (curr->y + next->y)* (curr->x * next->y - curr->y * next->x);
  }
  sumX = 1/(6 * polygon_area(polygon)) * sumX;
  sumY = 1/(6 * polygon_area(polygon)) * sumY;
  centroid.x = sumX;
  centroid.y = sumY;
  return centroid;
}

void polygon_translate(vec_list_t *polygon, vector_t translation){
  size_t size = vec_list_size(polygon);
  for (size_t i = 0; i < size; i++) {
    vector_t *curr = vec_list_get(polygon, i);
    curr->x = curr->x + translation.x;
    curr->y = curr->y + translation.y;
  }
}

void polygon_rotate(vec_list_t *polygon, double angle, vector_t point){
  size_t size = vec_list_size(polygon);
  for (size_t i = 0; i < size; i++) {
    vector_t *curr = vec_list_get(polygon, i);
    curr->x = curr->x - point.x;
    curr->y = curr->y - point.y;
    double newx = curr->x * cos(angle) - curr->y * sin(angle);
    double newy = curr->x * sin(angle) + curr->y * cos(angle);
    curr->x = newx + point.x;
    curr->y = newy + point.y;
  }
}

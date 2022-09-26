#include "shapes.h"
#include "test_util.h"
#include <assert.h>
#include <math.h>

const size_t POINT_SIZE = 4;
const vector_t VEC_ONE = {1, 1};
const vector_t VELOCITY = {20, 20};
const vector_t NEW_VEL = {50, 50};
const float RED = 1;
const float GREEN = 0;
const float BLUE = 0;

list_t *make_square(){
  list_t *square = list_init(4, free);
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t) {+1, +1};
  list_add(square, v);
  v = malloc(sizeof(*v));
  *v = (vector_t) {-1, +1};
  list_add(square, v);
  v = malloc(sizeof(*v));
  *v = (vector_t) {-1, -1};
  list_add(square, v);
  v = malloc(sizeof(*v));
  *v = (vector_t) {+1, -1};
  list_add(square, v);
  return square;
}

shape_t *make_shape(){
  shape_t *new_shape = shape_init(POINT_SIZE, VELOCITY, RED, GREEN, BLUE);
  return new_shape;
}

void test_vectors(){
  shape_t *sq_shape = make_shape();
  set_vectors(sq_shape, make_square());
  list_t *shape = get_vectors(sq_shape);
  assert(vec_equal(*(vector_t *) list_get(shape, 0), (vector_t) {+1, +1}));
  assert(vec_equal(*(vector_t *) list_get(shape, 1), (vector_t) {-1, +1}));
  assert(vec_equal(*(vector_t *) list_get(shape, 2), (vector_t) {-1, -1}));
  assert(vec_equal(*(vector_t *) list_get(shape, 3), (vector_t) {+1, -1}));
  shape_free(sq_shape);
  shape_t *sq_shape2 = make_shape();
  list_t *shape2 = list_init(2, free);
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t) {-2, -2};
  list_add(shape2, v);
  v = malloc(sizeof(*v));
  *v = (vector_t) {3, 4};
  list_add(shape2, v);
  set_vectors(sq_shape2, shape2);
  list_t *new_shape = get_vectors(sq_shape2);
  assert(vec_equal(*(vector_t *) list_get(new_shape, 0), (vector_t) {-2, -2}));
  assert(vec_equal(*(vector_t *) list_get(new_shape, 1), (vector_t) {3, 4}));
  //list_free(new_shape);
  //list_free(shape2);
  shape_free(sq_shape2);
}


void test_velocity(){
  shape_t *sq_shape = make_shape();
  //vector_t vel1 = get_vel(sq_shape);
  set_velocity(sq_shape, VEC_ONE);
  vector_t vel2 = get_vel(sq_shape);
  //assert(vec_equal(vel1, VEC_ZERO));
  assert(vec_equal(vel2, VEC_ONE));
  shape_free(sq_shape);
}

void test_get_colors(){
  shape_t *sq_shape = make_shape();
  assert(get_red(sq_shape) == RED);
  assert(get_green(sq_shape) == GREEN);
  assert(get_blue(sq_shape) == BLUE);
  shape_free(sq_shape);
}


int main(int argc, char *argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_velocity)
    DO_TEST(test_get_colors)
    DO_TEST(test_vectors)


    puts("shape_test PASS");
}

#include "scene.h"
#include "test_util.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

const double MASS = 10.0;
const rgb_color_t COLOR = {0.5, 0.5, 0.5};

//makes a square body
body_t *make_square_body(){
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
  body_t *sq = body_init(square, MASS, COLOR);
  return sq;
}
//makes a scene with a single square body
scene_t *make_single_scene(){
  scene_t *new = scene_init();
  body_t *body = make_square_body();
  scene_add_body(new, body);
  return new;
}

void test_scene_get_body(){
  scene_t *s = make_single_scene();
  body_t *body = scene_get_body(s, 0);
  list_t *shape = body_get_shape(body);
  assert(vec_equal(*(vector_t *) list_get(shape, 0), (vector_t) {+1, +1}));
  assert(vec_equal(*(vector_t *) list_get(shape, 1), (vector_t) {-1, +1}));
  assert(vec_equal(*(vector_t *) list_get(shape, 2), (vector_t) {-1, -1}));
  assert(vec_equal(*(vector_t *) list_get(shape, 3), (vector_t) {+1, -1}));
  scene_free(s);
  list_free(shape);
}
//stress test
list_t *simple_shape(){
  list_t *list = list_init(1, free);
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t) {+1, +1};
  list_add(list, v);
  return(list);
}
//scene with many bodies
scene_t *make_big_scene(){
  scene_t *b = scene_init();
  for (int i = 0; i < 1000; i++){
    body_t *body = body_init(simple_shape(), MASS, COLOR);
    scene_add_body(b, body);
  }
  return b;
}

void test_bodies(){
  scene_t *b = make_big_scene();
  for (int i = 0; i < 1000; i++){
    if (i % 20 == 0){
      body_t *next = scene_get_body(b, i + 1);
      list_t *next_shape = body_get_shape(next);
      scene_remove_body(b, i);
      body_t *curr = scene_get_body(b, i);
      list_t *curr_shape = body_get_shape(curr);
      assert(vec_isclose(*(vector_t *) list_get(next_shape, 0), *(vector_t *) list_get(curr_shape, 0)));
      list_free(next_shape);
      list_free(curr_shape);
    }
  }
  scene_free(b);
}


int main(int argc, char *argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_scene_get_body)
    DO_TEST(test_bodies)

    puts("scene_test PASS");
}

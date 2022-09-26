#include "body.h"
#include "test_util.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

const double MASS = 10.0;
const vector_t VEC_ONE = {1.0, 1.0};
const rgb_color_t COLOR = {0.5, 0.5, 0.5};
const double TO_ROTATE = M_PI / 4;

//makes a square at (+/- 1, +/-1)
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

//make a new body with a square as its shape
body_t *make_body(){
  body_t *new_body = body_init(make_square(), MASS, COLOR);
  return new_body;
}

void test_body_get_mass(){
  body_t *sq_body = make_body();
  assert(body_get_mass(sq_body) == MASS);
  body_free(sq_body);
}

void test_body_get_shape(){
  body_t *sq_body = make_body();
  list_t *shape = body_get_shape(sq_body);
  assert(vec_equal(*(vector_t *) list_get(shape, 0), (vector_t) {+1, +1}));
}

void test_body_centroid(){
  body_t *sq_body = make_body();
  vector_t centroid = body_get_centroid(sq_body);
  assert(vec_isclose(centroid, VEC_ZERO));
  body_set_centroid(sq_body, VEC_ONE);
  vector_t new_centroid = body_get_centroid(sq_body);
  assert(vec_isclose(new_centroid, VEC_ONE));
  body_free(sq_body);
}

void test_body_velocity(){
  body_t *sq_body = make_body();
  vector_t vel1 = body_get_velocity(sq_body);
  body_set_velocity(sq_body, VEC_ONE);
  vector_t vel2 = body_get_velocity(sq_body);
  assert(vec_equal(vel1, VEC_ZERO));
  assert(vec_equal(vel2, VEC_ONE));
  body_free(sq_body);
}

void test_body_rotate(){
  body_t *sq_body = make_body();
  list_t *shape1 = body_get_shape(sq_body);
  vector_t centroid = body_get_centroid(sq_body);
  body_set_rotation(sq_body, TO_ROTATE);
  list_t *shape2 = body_get_shape(sq_body);
  polygon_rotate(shape1, TO_ROTATE, centroid);
  for(int i = 0; i < 4; i++){
    vector_t *shape1_vec = (vector_t *) list_get(shape1, i);
    vector_t *shape2_vec = (vector_t *) list_get(shape2, i);
    assert(shape1_vec->x == shape2_vec->x);
    assert(shape1_vec->y == shape2_vec->y);
  }
  body_free(sq_body);
  list_free(shape1);
  list_free(shape2);
}

#define CIRC_NPOINTS 1000000
#define CIRC_AREA (CIRC_NPOINTS * sin(2 * M_PI / CIRC_NPOINTS) / 2)

// Circle with many points (stress test)
list_t *make_big_circ() {
    list_t *c = list_init(CIRC_NPOINTS, free);
    for (size_t i = 0; i < CIRC_NPOINTS; i++) {
        double angle = 2 * M_PI * i / CIRC_NPOINTS;
        vector_t *v = malloc(sizeof(*v));
        *v = (vector_t) {cos(angle), sin(angle)};
        list_add(c, v);
    }
    return c;
}

body_t *make_circ_body(){
  body_t *new_body = body_init(make_big_circ(), MASS, COLOR);
  return new_body;
}

void test_circ_centroid() {
    body_t *c = make_circ_body();
    assert(vec_isclose(body_get_centroid(c), VEC_ZERO));
    body_free(c);
}

void test_circ_translate() {
    body_t *c = make_circ_body();
    body_set_centroid(c, (vector_t) {100, 200});

    for (size_t i = 0; i < CIRC_NPOINTS; i++) {
        list_t *shape = body_get_shape(c);
        double angle = 2 * M_PI * i / CIRC_NPOINTS;
        assert(vec_isclose(
            * (vector_t *) list_get(shape, i),
            (vector_t) {100 + cos(angle), 200 + sin(angle)}
        ));
        list_free(shape);
    }
    assert(vec_isclose(body_get_centroid(c), (vector_t) {100, 200}));

    body_free(c);
}

void test_circ_rotate() {
    // Rotate about the origin at an unusual angle
    const double ROT_ANGLE = 0.5;

    body_t *c = make_circ_body();
    body_set_rotation(c, ROT_ANGLE);

    for (size_t i = 0; i < CIRC_NPOINTS; i++) {
        list_t *shape = body_get_shape(c);
        double angle = 2 * M_PI * i / CIRC_NPOINTS;
        assert(vec_isclose(
            *(vector_t *) list_get(shape, i),
            (vector_t) {cos(angle + ROT_ANGLE), sin(angle + ROT_ANGLE)}
        ));
        list_free(shape);
    }
    assert(vec_isclose(body_get_centroid(c), VEC_ZERO));

    body_free(c);
}

// Weird nonconvex polygon

body_t *make_weird() {
    list_t *w = list_init(5, free);
    vector_t *v = malloc(sizeof(*v));
    *v = VEC_ZERO;
    list_add(w, v);
    v = malloc(sizeof(*v));
    *v = (vector_t) {4, 1};
    list_add(w, v);
    v = malloc(sizeof(*v));
    *v = (vector_t) {-2, 1};
    list_add(w, v);
    v = malloc(sizeof(*v));
    *v = (vector_t) {-5, 5};
    list_add(w, v);
    v = malloc(sizeof(*v));
    *v = (vector_t) {-1, -8};
    list_add(w, v);
    body_t *w_body = body_init(w, MASS, COLOR);
    return w_body;
}

void test_weird_centroid() {
    body_t *w = make_weird();
    assert(vec_isclose(body_get_centroid(w), (vector_t) {-223.0 / 138.0, -51.0 / 46.0}));
    body_free(w);
}

void test_weird_translate() {
    body_t *w = make_weird();
    body_set_centroid(w, (vector_t) {-10, -20});

    list_t *shape = body_get_shape(w);
    assert(vec_isclose(*(vector_t *) list_get(shape, 0), (vector_t) {-10, -20}));
    assert(vec_isclose(*(vector_t *) list_get(shape, 1), (vector_t) {-6,  -19}));
    assert(vec_isclose(*(vector_t *) list_get(shape, 2), (vector_t) {-12, -19}));
    assert(vec_isclose(*(vector_t *) list_get(shape, 3), (vector_t) {-15, -15}));
    assert(vec_isclose(*(vector_t *) list_get(shape, 4), (vector_t) {-11, -28}));
    assert(vec_isclose(body_get_centroid(w), (vector_t) {-1603.0 / 138.0, -971.0 / 46.0}));

    body_free(w);
    list_free(shape);
}

int main(int argc, char *argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_body_get_mass)
    DO_TEST(test_body_get_shape)
    DO_TEST(test_body_centroid)
    DO_TEST(test_body_velocity)
    DO_TEST(test_body_rotate)
    DO_TEST(test_circ_centroid)
    DO_TEST(test_circ_translate)
    DO_TEST(test_circ_rotate)
    DO_TEST(test_weird_centroid)
    DO_TEST(test_weird_translate)

    puts("body_test PASS");
}

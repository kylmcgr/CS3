#include "forces.h"

// creates different collisions between scene bodies
// -- can call on patronus_dementor_collision if the collision is on either a
//    patronus or dementor body
// -- calls various apply collision handlers that vary based on purpose
void collision(scene_t *scene, body_t *body, int power);

// calculates the velocity at a boundary and applies the gravity to the
// dementors and powerups at each tick
void gravity(scene_t *scene, double dt, double gravity);

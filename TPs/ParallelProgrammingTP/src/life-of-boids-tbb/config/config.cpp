#include "config.h"

size_t DEFAULT_NUM_BIRDS = 1000;
const size_t DEFAULT_NUM_PREDATORS = 0;
const size_t DEFAULT_NUM_OBSTACLES = 0;
const size_t DEFAULT_NUM_TREES = 0;
const int DEFAULT_NUM_FRUITS_DROPS = 0;
const Real CLOSE = 50;
const Real SPEED = 1;
const Real PRED_SPEED = 0.5 * SPEED;
const Real PI  = 3.14159265358979323846;
const Real VIEW_RANGE = 3*PI/2;
const Real HALF = 0.5;

// Display parameters
const int WIDTH = 2400;
const int HEIGHT = 1300;
const int MAX_OBSTACLE_SIZE = HEIGHT/5;
const int MAX_FRUIT_TREE_SIZE = 50;
const int MIN_FRUIT_TREE_SIZE = 25;
const Real MAX_FRUIT_DISTANCE = 75;


const Real RATIO = (float)WIDTH / (float)HEIGHT;
const Real BODY_SIZE = 0.015;
const Real BODY_OVERLAP = (Real)((sqrt(3)*BODY_SIZE/2)*((Real)WIDTH/2));
const vec3 PRED_COLOR = { 1., 0., 0. };
const vec3 BIRD_COLOR = { 1., 1., .7 };
const vec3 OBSTACLE_COLOR = { 0.2, 0.2, 0.2 };
const vec3 TREE_COLOR = { (Real)98/(Real)256, (Real)52/(Real)256, (Real)18/(Real)256 };
const vec3 FRUIT_COLOR = {0., 1., 0.};
const int NUMBER_LOOP_FPS = 10;

// Laws parameters
const Real SEPARATION_RANGE = (Real)(2 * sqrt(5) * BODY_SIZE * ((Real)WIDTH) / 9);
const Real DEAD_RANGE = SEPARATION_RANGE / 4;
const Real ALIGNMENT_RANGE = 3 * SEPARATION_RANGE;
const Real COHESION_RANGE = 2 * ALIGNMENT_RANGE;
const Real PREDATOR_RANGE = COHESION_RANGE;
const Real FRUIT_RANGE = COHESION_RANGE;
const Real IGNORE_RANGE = 5 * COHESION_RANGE;

const Real SEPARATION_RELAXATION = 0.75;
const Real ALIGNMENT_RELAXATION = 0.9;
const Real COHESION_RELAXATION = 0.9;
const Real PREDATOR_RELAXATION = 0.9;
const Real OBSTACLE_RELAXATION = 0.5;

const int FRUIT_TIME_MAX = 25;
const int FRUIT_TIME_MIN = 0;

const int N_THREADS = 4;

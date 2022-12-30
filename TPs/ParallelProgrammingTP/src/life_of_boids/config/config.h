#pragma once
#include <cstdio>
#include <array>
#include <cmath>
#include <chrono>

#define agents_t std::unordered_map<size_t, Agent>

using Real = float;
using vec2 = std::array<Real, 2>;
using vec3 = std::array<Real, 3>;
using vec4 = std::array<Real, 4>;
using mat4x4 = std::array<vec4, 4>;

// Agent parameters
extern const size_t DEFAULT_NUM_BIRDS;
extern const size_t  DEFAULT_NUM_PREDATORS;
extern const size_t  DEFAULT_NUM_OBSTACLES;
extern const size_t  DEFAULT_NUM_TREES;
extern const int  DEFAULT_NUM_FRUITS_DROPS;
extern const Real CLOSE;
extern const Real SPEED;
extern const Real PRED_SPEED;
extern const Real PI;
extern const Real VIEW_RANGE;
extern const Real HALF;

// Display parameters
extern const int WIDTH;
extern const int HEIGHT;
extern const int MAX_OBSTACLE_SIZE;
extern const int MAX_FRUIT_TREE_SIZE;
extern const int MIN_FRUIT_TREE_SIZE;
extern const Real MAX_FRUIT_DISTANCE;

extern const Real RATIO;
extern const Real BODY_SIZE;
extern const Real BODY_OVERLAP;
extern const vec3 PRED_COLOR;
extern const vec3 BIRD_COLOR;
extern const vec3 OBSTACLE_COLOR;
extern const vec3 TREE_COLOR;
extern const vec3 FRUIT_COLOR;
extern const int NUMBER_LOOP_FPS;

// Laws parameters
extern const Real SEPARATION_RANGE;
extern const Real DEAD_RANGE;
extern const Real ALIGNMENT_RANGE;
extern const Real COHESION_RANGE;
extern const Real PREDATOR_RANGE;
extern const Real FRUIT_RANGE;
extern const Real IGNORE_RANGE;

extern const Real SEPARATION_RELAXATION;
extern const Real ALIGNMENT_RELAXATION;
extern const Real COHESION_RELAXATION;
extern const Real PREDATOR_RELAXATION;
extern const Real OBSTACLE_RELAXATION;

extern const int FRUIT_TIME_MAX;
extern const int FRUIT_TIME_MIN;

extern const int N_THREADS;
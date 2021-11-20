#pragma once
#include "../model/Flock.hpp"
#include "Color.h"


//Creates a complex bird flock with all options, including the actual flock
//ALL flocks are in same position
Flock create_bird_flock(int size, Color agent_color, int agent_size, int agent_range, int agent_angle_view, Vec2 agent_position, Vec2 agent_velocity, float sep_relax, float cohe_relax, float align_relax, float max_speed);

//Same with random positions
Flock create_bird_flock(int size, Color agent_color, int agent_size, int agent_range, int agent_angle_view, float sep_relax, float cohe_relax, float align_relax, float max_speed);

//Generates a flock with random colors for each agent
Flock created_bird_flock_random_colors(int size, int agent_size, int agent_range, int agent_angle_view, float sep_relax, float cohe_relax, float align_relax, float speed_relax);

//everything randomized
Flock generate_fully_random_bird_flock();

//everything randomized but in between the values given by the user
Flock generate_random_bird_flock(Vec2 inf_sup_size, Vec2 inf_sup_range, Vec2 inf_sup_agent_size, Vec2 inf_sup_agent_angle_view, Vec2 inf_sup_agent_position, Vec2 inf_sup_agent_velocity, Vec2 inf_sup_speed_relax);

//angle view is about 300, range 50
Flock generate_parrot_flock(int size);

//angle view is 270, range is 50
Flock generate_dove_flock(int size);

//view 360 and range 70
Flock generate_duck_flock(int size);

Flock generate_ant_flock(int size);

Flock generate_fly_flock(int size);

Color random_color();

Flock generate_flock_with_args(int argc, char* argv[]);
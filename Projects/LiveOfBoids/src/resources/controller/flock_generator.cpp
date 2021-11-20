#include "flock_generator.hpp"
#include "../model/Flock.hpp"
#include "../model/Bird.hpp"
#include "../../lib/myLaws/AlignmentLaw.hpp"
#include "../../lib/myLaws/CohesionLaw.hpp"
#include "../../lib/myLaws/SeparationLaw.hpp"
#include <vector>
#include <string>

Flock create_bird_flock(int size, Color agent_color, int agent_size, int agent_range, int agent_angle_view, Vec2 agent_position, Vec2 agent_velocity, float sep_relax, float cohe_relax, float align_relax, float maxSpeed)
{
	CohesionLaw coLaw(cohe_relax);
	SeparationLaw sepLaw(sep_relax);
	AlignmentLaw aligLaw(align_relax);

	std::vector<Agent*> agents;

	int reserve_size = size * 2;
	agents.reserve(reserve_size);

	for (int i = 0; i < size; ++i) {
		agents.push_back(new Bird{ agent_position, agent_velocity, agent_size, agent_angle_view, agent_range, maxSpeed, agent_color, coLaw, aligLaw, sepLaw });
	}

	return Flock{ agents };
};

Flock created_bird_flock_random_colors(int size, int agent_size, int agent_range, int agent_angle_view, float sep_relax, float cohe_relax, float align_relax, float speed_relax) {
	std::vector<Agent*> agents;

	CohesionLaw coLaw(cohe_relax);
	SeparationLaw sepLaw(sep_relax);
	AlignmentLaw aligLaw(align_relax);

	int reserve_size = size * 2;
	agents.reserve(reserve_size);

	for (int i = 0; i < size; ++i) {
		agents.push_back(new Bird{ randomVec2Generation(-500, 500), randomVec2Generation(-10, 10), agent_size, agent_angle_view, agent_range, speed_relax, random_color(), coLaw, aligLaw, sepLaw });
	}

	return Flock{ agents };
}

Flock create_bird_flock(int size, Color agent_color, int agent_size, int agent_range, int agent_angle_view, float sep_relax, float cohe_relax, float align_relax, float maxSpeed) {
	std::vector<Agent*> agents;

	CohesionLaw coLaw(cohe_relax);
	SeparationLaw sepLaw(sep_relax);
	AlignmentLaw aligLaw(align_relax);

	int reserve_size = size * 2;
	agents.reserve(reserve_size);

	for (int i = 0; i < size; ++i) {
		agents.push_back(new Bird{ randomVec2Generation(-500, 500), randomVec2Generation(-10, 10), agent_size, agent_angle_view, agent_range, maxSpeed, agent_color, coLaw, aligLaw, sepLaw });
	}

	return Flock{ agents };
}

Flock generate_fully_random_bird_flock()
{
	int flock_size = (int)random_float(20, 5000);
	int range;
	int angle_view;
	int agent_size;
	float cohesion_relax;
	float sep_relax;
	float align_relax;
	float speed_relax;
	Vec2 position;
	Vec2 velocity;
	Color agent_color = Color::Default;

	std::vector<Agent*> agents;

	int reserve_size = flock_size * 2;
	agents.reserve(reserve_size);

	for (int i = 0; i < flock_size; ++i) {
		range = (int)random_float(5, 100);
		angle_view = (int)random_float(100, 360);
		agent_size = (int)random_float(2, 20);
		position = randomVec2Generation(0, 500);
		velocity = randomVec2Generation(-10, 10);
		speed_relax = random_float(1, 30);
		align_relax = random_float(-2, 2);
		sep_relax = random_float(-2, 2);
		cohesion_relax = random_float(0, 1);
		agent_color = random_color();

		agents.push_back(new Bird{ position, velocity, agent_size, angle_view, range, speed_relax, agent_color, CohesionLaw(cohesion_relax), AlignmentLaw(align_relax), SeparationLaw(sep_relax) });
	}

	return Flock{ agents };
}

Flock generate_random_bird_flock(Vec2 inf_sup_size, Vec2 inf_sup_range, Vec2 inf_sup_agent_size, Vec2 inf_sup_agent_angle_view, Vec2 inf_sup_speed_relax)
{
	int flock_size = (int)random_float((int)inf_sup_size.x, (int)inf_sup_size.y);
	int range = (int)random_float((int)inf_sup_range.x, (int)inf_sup_range.y);
	int angle_view = (int)random_float((int)inf_sup_agent_angle_view.x, (int)inf_sup_agent_angle_view.y);
	int agent_size = (int)random_float((int)inf_sup_agent_size.x, (int)inf_sup_agent_size.y);
	float cohesion_relax = 1 / random_float(-10, 10);
	float sep_relax = 1 / random_float(-10, 10);
	float align_relax = 1 / random_float(-10, 10);
	float speed_relax = random_float((int)inf_sup_speed_relax.x, (int)inf_sup_speed_relax.y);


	//Change to random color 
	return create_bird_flock(flock_size, Color::Default, agent_size, range, angle_view, sep_relax, cohesion_relax, align_relax, speed_relax);
}

//TODO : Change 
Flock generate_parrot_flock(int size)
{
	float cohesion_relax = 0.0002f;
	float sep_relax = 0.9f;
	float align_relax = 0.3f;
	return created_bird_flock_random_colors(size, 6, 50, 320, sep_relax, cohesion_relax, align_relax, 5.f);
}

Flock generate_dove_flock(int size) {
	float cohesion_relax = 0.0004f;
	float sep_relax = 1.f;
	float align_relax = 0.05f;
	return create_bird_flock(size, Color::White, 6, 50, 270, sep_relax, cohesion_relax, align_relax, 10.f);
}

Flock generate_duck_flock(int size)
{
	float cohesion_relax = 0.00008f;
	float sep_relax = 0.5f;
	float align_relax = 0.9f;
	return create_bird_flock(size, Color::Green, 6, 70, 360, sep_relax, cohesion_relax, align_relax, 5.f);
}

//TODO : Change color to black or grey
Flock generate_ant_flock(int size) {
	float cohesion_relax = 0.0000008f;
	float sep_relax = 2.5f;
	float align_relax = -1.f;
	return create_bird_flock(size, Color::Grey, 4, 20, 360, sep_relax, cohesion_relax, align_relax, 10.f);
}

Flock generate_fly_flock(int size) {
	float cohesion_relax = 0.08f;
	float sep_relax = 1.f;
	float align_relax = 0.2f;
	return create_bird_flock(size, Color::White, 4, 40, 360, sep_relax, cohesion_relax, align_relax, 9.f);
}

Color random_color() {

	int randomNb = (int)random_float(0, 6);
	Color result;

	switch (randomNb)
	{
	case 0:
		result = Color::Blue;
		break;

	case 1:
		result = Color::Green;
		break;

	case 2:
		result = Color::White;
		break;

	case 3:
		result = Color::Default;
		break;

	case 4:
		result = Color::Purple;
		break;

	case 5:
		result = Color::Yellow;
		break;

	case 6:
		result = Color::Grey;
		break;

	default:
		result = Color::Default;
		break;
	}

	return result;
}

//Sert a convertir une chaine de caractere en entier pour le comparer dans un switch case
//On va assigner une valeur à chaque caractere et créer un int unique pour chaque chaine de caractere
constexpr unsigned int str2int(const char* str, int h = 0)
{
	return !str[h] ? 0/*5381*/ : (str2int(str, h + 1) /** 33*/) ^ str[h];
}

Color str_to_color(char* color) {

	Color result;

	switch (str2int(color))
	{
	case str2int("Blue"):
	case str2int("blue"):
		result = Color::Blue;
		break;

	case str2int("Green"):
	case str2int("green"):
		result = Color::Green;
		break;

	case str2int("White"):
	case str2int("white"):
		result = Color::White;
		break;

	case str2int("Purple"):
	case str2int("purple"):
		result = Color::Purple;
		break;

	case str2int("Yellow"):
	case str2int("yellow"):
		result = Color::Yellow;
		break;

	case str2int("Grey"):
	case str2int("grey"):
		result = Color::Grey;
		break;
	case str2int("red"):
	case str2int("Red"):
		result = Color::Red;
		break;
	default:
		result = Color::Default;
		break;
	}

	return result;
}

Flock generate_flock_with_args(int argc, char* argv[])
{

	switch (argc)
	{
	case 1://no arguments
		std::cout << "\nGenerating a flock of 600 parrots. To change the flock's type, use arguments ./life-of-boids.exe SIZE TYPE\nAvailable types can be found in the readme file.\nFor a more detailed configuration of the flock, read the readme file.\n\n" << std::endl;
		return generate_parrot_flock(600);

	case 2:
		//Only flock size, or fully random if arg is 'r'
		if (*(argv[1]) == 'r') {
			return generate_fully_random_bird_flock();
		}
		else if (*(argv[1]) == 'f') {
			float cohesion_relax = 0.0004f;
			float sep_relax = 1.f;
			float align_relax = 0.05f;
			return create_bird_flock(600, Color::White, 6, 50, 270, Vec2{ 250, 250 }, Vec2{ 1,1 }, sep_relax, cohesion_relax, align_relax, 8.f);
		}
		else {
			int size = atoi(argv[1]);

			if (size > 0)
				return generate_parrot_flock(size);
			else
				throw std::invalid_argument("Error : argument should be an int > 0 (size of the flock).");
		}
	case 3:
	{
		//only flock size and type of birds
		int size = atoi(argv[1]);
		char* bird_type = argv[2];

		if (size <= 0)
			throw std::invalid_argument("Error : first argument is size and should be an int > 0");

		switch (str2int(bird_type))
		{
		case str2int("parrot"):
		case str2int("Parrot"):
			return generate_parrot_flock(size);
		case str2int("ant"):
		case str2int("Ant"):
			return generate_ant_flock(size);
		case str2int("fly"):
		case str2int("Fly"):
			return generate_fly_flock(size);
		case str2int("dove"):
		case str2int("Dove"):
			return generate_dove_flock(size);
		case str2int("duck"):
		case str2int("Duck"):
			return generate_duck_flock(size);
		default:
			throw std::invalid_argument("Error : second argument is bird type, read the readme file to know what types are available.");
		}


	}
	case 10:
	{
		//flock size, color, bird size, range , angle of view + relaxation alignment, separation, cohesion, and max speed
		int size = atoi(argv[1]);
		if (size <= 0) {
			size = 0;
			std::cout << "WARNING : Your flock's size will be 0, check your arguments." << std::endl;
		}

		char* pColor = argv[2];
		Color color = str_to_color(pColor);
		if (color == Color::Default) {
			std::cout << "WARNING : Color unknown, default color will be applied" << std::endl;
		}

		int bird_size = atoi(argv[3]);

		if (bird_size <= 0) {
			bird_size = 0;
			std::cout << "WARNING : Agent's size will be 0, check your arguments." << std::endl;
		}

		int range = atoi(argv[4]);
		if (range <= 0) {
			range = 0;
			std::cout << "WARNING : Agent's range of view will be 0, check your arguments." << std::endl;
		}
		int angle_view = atoi(argv[5]);
		if (angle_view <= 0) {
			angle_view = 0;
			std::cout << "WARNING : Agent's angle view will be 0, check your arguments." << std::endl;
		}

		float r_align = std::stof(argv[6]);
		float r_sep = std::stof(argv[7]);
		float r_cohe = std::stof(argv[8]);

		float max_speed = std::stof(argv[9]);
		if (max_speed <= 0) {
			max_speed = 0;
			std::cout << "WARNING :Agent's max speed (int) will be 0, they won't move, check your arguments." << std::endl;
		}

		std::cout << "\nGenerating a flock with\n\t" << size << " " << pColor << " agents\n\trange : " << range << " / angle view : " << angle_view << '\n';
		std::cout << "\tmax speed : " << max_speed <<"\n\n\t >> Alignment : " << r_align << "\n\t >> Separation : " << r_sep << "\n\t >> Cohesion : " << r_cohe << "\n\n" << std::endl;
		
		return create_bird_flock((int)size, color, (int)bird_size, (int)range, (int)angle_view, r_sep, r_cohe, r_align, max_speed);
	}
	default:
		//error
		std::cout << ">> Error : Number of argument can only be 0, 1, 2 or 9.\n>> Read the readme file to know what are the arguments.\n";
		throw std::domain_error("Wrong number of arguments");
	}
}


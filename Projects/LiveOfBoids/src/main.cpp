#include <iostream>
#include "resources/model/Flock.hpp"
#include "resources/graphics/GraphicalManager.hpp"
#include "resources/controller/flock_generator.hpp"
#define CRITICAL_FLOCK_SIZE 2500 //at this point, we start to have fps issues
Flock* MAIN_pFLOCK = nullptr;

int main(int argc, char* argv[]) {

	try
	{
		Flock flock = generate_flock_with_args(argc, argv);

		bool fullScreen = false;
		GraphicalManager GM{ Color::BlueGrey, fullScreen };

		if (flock.getPopSize() > 700) {
			flock.optimized_computing = true;
			std::cout << "Size of flock over 700, using the optimized version of compute neighbors.\nIf you want to use predators, you might prefer generating a small flock (size under 700). Optimized version is in beta." << std::endl;
		}
		if (flock.getPopSize() > CRITICAL_FLOCK_SIZE) {
			std::cout << "\nWARNING : Flock size is over "<< CRITICAL_FLOCK_SIZE <<", the program might have frame rate issues." << std::endl;
		}

		MAIN_pFLOCK = &flock;

		float t = 0;
		bool shouldClose = false;
		do {
			shouldClose = GM.mainLoop();
			++t;
		} while (!shouldClose);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}
	return 0;
}

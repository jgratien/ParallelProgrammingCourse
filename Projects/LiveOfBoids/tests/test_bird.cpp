#include <iostream>
#include "gtest/gtest.h"
#include "../src/resources/model/Bird.hpp"
#include "../src/resources/model/Agent.hpp"
#include "../src/resources/model/Flock.hpp"
#include <vector>
#include "../src/resources/controller/flock_generator.hpp"

namespace {
	TEST(TestBird, Instanciation) {
		//Vec2 pos = Vec2(1, 1);
		//Vec2 vel = Vec2(0, 1);
		//Bird b = Bird(pos, vel);

		//ASSERT_EQ(b.getPosition().x, 1);
		//ASSERT_EQ(b.getPosition().y, 1);
		//ASSERT_EQ(b.getNextPosition().x, 1);
		//ASSERT_EQ(b.getNextPosition().y, 1);
		//ASSERT_EQ(b.getVelocity().x, 0);
		//ASSERT_EQ(b.getVelocity().y, 1);
		//ASSERT_EQ(b.getNextVelocity().x, 0);
		//ASSERT_EQ(b.getNextVelocity().y, 1);

		//b.computePosition();
		//ASSERT_EQ(b.getNextPosition().x, 1);
		//ASSERT_EQ(b.getNextPosition().y, 2);

		//b.updatePosition();
		//ASSERT_EQ(b.getPosition().x, 1);
		//ASSERT_EQ(b.getPosition().y, 2);
	}


	TEST(TestFlock, InstanciationAndAddingAgents) {
		int testSize = 300;
		int loopSize = 1000;

		//Flock flock(testSize);
		Flock flock = generate_dove_flock(testSize);
		//flock.print();
		//Initializing a flock with pop size 300 and checking size of vector population
		ASSERT_EQ(flock.getPopSize(), testSize) << "Population size should be " << testSize;

		//Adding one agent and checking size
		Bird b1 = Bird{};
		flock.addAgent(&b1);
		ASSERT_EQ(flock.getPopSize(), testSize + 1) << "Added an agent with addAgent(), now popSize should be " << testSize << " +1";

		//Adding 1000 agents
		for (int i = 0; i < loopSize; ++i) {
			Bird bx = Bird{};
			flock.addAgent(&bx);
		}
		ASSERT_EQ(flock.getPopSize(), testSize + loopSize+1) << "Added " << loopSize << " agents, now popSize should be " << testSize << " + " << loopSize << "+ 1";


		//Adding a specific agent with specific values
		Bird b = Bird(Vec2(0, 0), Vec2(0, 0));
		flock.addAgent(&b);

		ASSERT_EQ(flock.getPopSize(), testSize + loopSize + 2) << "Added one more agents, should be " << testSize << " + " << loopSize << "+ 2";
		//flock.print();
		//flock[testSize + loopSize + 1].getPosition()
		Vec2 pos = (*flock.getAgent(testSize + loopSize + 1)).getPosition();
		Vec2 vel = (*flock.getAgent(testSize + loopSize + 1)).getVelocity();
		EXPECT_EQ(pos, b.getPosition()) << "Last agent's position should be 0,0";
		EXPECT_EQ(vel, b.getVelocity()) << "Last agent's velocity should be 0,0";

		//flock.addAgent(b);
		//flock.addAgent(b);
		//flock.addAgent(b);
		//flock.addAgent(b);
		//flock.addAgent(b);

		//std::vector<Bird> neighbors = flock.computeNeighbors(flock.getAgent(flock.getPopSize() - 1), 1, 1);
		//EXPECT_GE(neighbors.size(), 5) << "Expected at least 5 neighbors to the bird we added";

		//Printing neighbors
		//for (Bird b : neighbors) {
		//	std::cout << " : Pos(" << b.getPosition().x << ", " << b.getPosition().y << ")  //  Vel(" << b.getVelocity().x << ", " << b.getVelocity().y << ")" << std::endl;
		//}
		//std::vector<Bird> neighbors = flock.computeNeighbors(flock.getAgent(1000)
	}

	TEST(TestFlock, Destroy) {
		std::vector<Agent*> mainFlock;

		mainFlock.push_back(new Bird(Vec2(1, 1), Vec2(1, 1)));
		mainFlock.push_back(new Bird(Vec2(2, 2), Vec2(2, 2)));
		mainFlock.push_back(new Bird(Vec2(3, 3), Vec2(3, 3)));
	
		Flock flock{ mainFlock };

		ASSERT_EQ(flock.getPopSize(), 3);
		//flock.print();
		flock.destroyLastAgent();
		//flock.print();
		ASSERT_EQ(flock.getPopSize(), 2) << "Error in destroyLastAgent: PopSize = " << flock.getPopSize();
		ASSERT_EQ((*flock.getAgent(1)).getPosition().x, 2);

		flock.addAgent(new Bird(Vec2(1.5, 1), Vec2(1, 1)));

		//flock.print();
		flock.setAgentsToBeDestroyed(Vec2(1, 1), 1);
		flock.destroyAgents();
		//flock.print();
		ASSERT_EQ(flock.getPopSize(), 1) << "Error in destroyLastAgent: PopSize = " << flock.getPopSize();
		ASSERT_EQ((*flock.getAgent(0)).getPosition().x, 2);

	}

	TEST(TestFlock, NeighborhoodComputing) {

	}
}

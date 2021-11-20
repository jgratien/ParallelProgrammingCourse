#include <iostream>
#include "gtest/gtest.h"
#include "../src/myMath/vec2.hpp"
#include "../src/Bird.hpp"
#include "../src/Flock.hpp"
#include <vector>

TEST(Bird, Methods) {
	Vec2 position = Vec2(1., 1.);
	Vec2 velocity = Vec2(0., 1.);
	Bird bird = Bird(position, velocity);
	ASSERT_NEAR(bird.getPosition().x, 1, 1e-6);
	ASSERT_NEAR(bird.getPosition().y, 1, 1e-6);
	ASSERT_NEAR(bird.getNextPosition().x, 1, 1e-6);
	ASSERT_NEAR(bird.getNextPosition().y, 1, 1e-6);
	ASSERT_NEAR(bird.getVelocity().x, 0, 1e-6);
	ASSERT_NEAR(bird.getVelocity().y, 1, 1e-6);
	ASSERT_NEAR(bird.getNextVelocity().x, 0, 1e-6);
	ASSERT_NEAR(bird.getNextVelocity().y, 1, 1e-6);

	bird.computePosition();
	ASSERT_NEAR(bird.getNextPosition().x, 1, 1e-6);
	ASSERT_NEAR(bird.getNextPosition().y, 2, 1e-6);

	bird.updatePosition();
	ASSERT_NEAR(bird.getPosition().x, 1, 1e-6);
	ASSERT_NEAR(bird.getPosition().y, 2, 1e-6);
	ASSERT_NEAR(bird.getVelocity().x, 0, 1e-6);
	ASSERT_NEAR(bird.getVelocity().y, 1, 1e-6);
	//ASSERT_NEAR(a.norm(), 5., 1e-3) << "Norm";
}

TEST(Bird, Methods) {
	Vec2 position = Vec2(1., 1.);
	Vec2 velocity = Vec2(0., 1.);
	Bird bird1 = Bird(position, velocity);
	Bird bird2 = Bird(position, velocity);
	std::vector<Bird> birdsVec{ bird1, bird2 };

}
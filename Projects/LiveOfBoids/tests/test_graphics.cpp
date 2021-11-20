#include <iostream>
#include "gtest/gtest.h"
#include "../src/resources/graphics/graphics.hpp"
#include "../src/resources/graphics/oglTypes.hpp"
#include "../src/resources/model/Bird.hpp"
#include "../src/resources/model/Agent.hpp"
#include "../src/lib/myMath/Vec2.hpp"
#include <vector>

namespace {
	TEST(TestGraphics, DrawAgent1) {
		mat2x6 result = drawAgent(Vec2(50, 50), Vec2(2, 2));
		ASSERT_EQ(result[0].x, 50.f);
		ASSERT_EQ(result[0].y, 50.f);
		ASSERT_NEAR(result[1].x, 54.949749f, 0.0001);
		ASSERT_NEAR(result[2].x, 40.100502f, 0.0001);
	}

	TEST(TestGraphics, DrawAgent2) {
		mat2x6 result = drawAgent(Vec2(100.f, 50.f), Vec2(-1.5678f, 3.72654f));
		ASSERT_EQ(result[0].x, 100.f);
		ASSERT_EQ(result[0].y, 50.f);
		ASSERT_NEAR(result[1].x, 97.285469f, 0.0001);
		ASSERT_NEAR(result[2].x, 96.262299f, 0.0001);
		ASSERT_NEAR(result[5].x, 109.166763f, 0.0001);
	}

}

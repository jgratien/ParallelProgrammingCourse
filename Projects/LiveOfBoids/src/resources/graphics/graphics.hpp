#pragma once

#include <vector>
#include <array>
#include "../../lib/myMath/Vec2.hpp"
#include "oglTypes.hpp"
#include "../model/Bird.hpp"

mat2x6 drawAgent(Vec2 position, Vec2 velocity);

Vec2 keepPositionInScreen(Vec2 position, float width, float height);
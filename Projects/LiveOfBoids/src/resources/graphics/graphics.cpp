#include "graphics.hpp"
#include "../../lib/myMath/Vec2.hpp"
#include "oglTypes.hpp"
#include "../model/Bird.hpp"

mat2x6 drawAgent(Vec2 position, Vec2 velocity) {
	float size = 7;
	float angle = velocity.angle();
	Vec2 front{ position + Vec2{ 1.f * size, 0 }.rotate(angle) } ;
	Vec2 backLeft{ position + Vec2{ - 1.f * size, 1.f * size }.rotate(angle) } ;
	Vec2 backRight{ position + Vec2{ -1.f * size, -1.f * size }.rotate(angle) } ;

	mat2x6 r = { position, front, backLeft, position, front, backRight };
	return r;
}


Vec2 keepPositionInScreen(Vec2 position, float width, float height) {
	float x = position.x;
	float y = position.y;
	if (x >= width) {
		x = x - width;
	}
	else if (x < 0) {
		x = x + width;
	}
	if (y >= height) {
		y =  y - height;
	}
	else if (y < 0) {
		y = y + height;
	}
	return Vec2(x, y);
}
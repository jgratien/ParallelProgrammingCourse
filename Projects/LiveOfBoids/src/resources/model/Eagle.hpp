#pragma once
#include "Agent.hpp"
#include "../../lib/myMath/Vec2.hpp"
#include "../../lib/myLaws/HuntingLaw.hpp"
#include "../../lib/myLaws/SeparationLaw.hpp"
#include <vector>

class Eagle : public Agent {
private:
	HuntingLaw m_huntingLaw;
	SeparationLaw m_separationLaw;

public:
	Eagle(); // Constructeur sans arguments
	Eagle(const Vec2& position, const Vec2& velocity);
	Eagle(const Vec2& position, const Vec2& velocity, const int& bodySize, const int& viewAngle, const int& range, const float& speedRelaxation, const Color& color);
	Eagle(const Vec2& position, const Vec2& velocity, const int& bodySize, const int& viewAngle, const int& range, const float& speedRelaxation, const Color& color, const HuntingLaw& huntLaw);

	void computeLaws(const std::vector<Agent*>& neighborsBird, const std::vector<Agent*>& neighborsPredator) override;

	//void computeLaws(const std::vector<Agent*>& neighbors) override;

	void print() const;

};

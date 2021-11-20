#pragma once
#include "Agent.hpp"
#include "../../lib/myMath/Vec2.hpp"
#include <vector>
#include "../../lib/myLaws/SeparationLaw.hpp"
#include "../../lib/myLaws/CohesionLaw.hpp"
#include "../../lib/myLaws/AlignmentLaw.hpp"



class Bird : public Agent{
private:
	CohesionLaw m_cohesionLaw;
	AlignmentLaw m_alignmentLaw;
	SeparationLaw m_separationLaw;

public:
	Bird(); // Constructeur sans arguments
	//Bird(const Bird& other);
	Bird(const Vec2& position, const Vec2& velocity);
	Bird(const Vec2& position, const Vec2& velocity, const int& bodySize, const int& viewAngle, const int& range, const float &speedRelaxation, const Color& color);
	Bird(const Vec2& position, const Vec2& velocity, const int& bodySize, const int& viewAngle, const int& range, const float &speedRelaxation, const Color& color, const CohesionLaw &cohesionLaw, const AlignmentLaw &alignmentLaw, const SeparationLaw &separationLaw);

	void computeLaws(const std::vector<Agent*>& neighborsBird, const std::vector<Agent*>& neighborsPredator) override;

	void print() const;

};
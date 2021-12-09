#include "SeparationLaw.hpp"
#include "../myMath/Vec2.hpp"
#include "../myMath/utils.hpp"
#include <vector>
#include "../../resources/model/Agent.hpp"


SeparationLaw::SeparationLaw(const float& relaxation) : Law(relaxation) {};
SeparationLaw::SeparationLaw() : Law(1.f) {};

Vec2 SeparationLaw::compute(Agent& currentAgent, const std::vector<Agent*> &neighbors) const {

	Vec2 result{};
	float distBetwA = 0, weight = 0;

	for (Agent *b : neighbors)
	{
		if (!((*b).getPosition() == currentAgent.getPosition())) {
			distBetwA = distance((*b).getPosition(), currentAgent.getPosition());
			weight = 1 / ((distBetwA) * (distBetwA));
			result = result + (currentAgent.getPosition() - (*b).getPosition()) * weight;
		}

	}

	return result * m_relaxation;

}
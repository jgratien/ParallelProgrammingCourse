#include "AlignmentLaw.hpp"
#include "../myMath/Vec2.hpp"
#include "../myMath/utils.hpp"
#include <vector>
#include "../../resources/model/Agent.hpp"

AlignmentLaw::AlignmentLaw(const float& relaxation) : Law(relaxation){};
AlignmentLaw::AlignmentLaw() : Law(1.f){};

Vec2 AlignmentLaw::compute(Agent&, const std::vector<Agent*>& neighbors) const {

	Vec2 nextVelocity(0, 0);
	if (neighbors.size() > 0) {
		for (int i = 0; i < (int)neighbors.size(); ++i) {
			nextVelocity = nextVelocity + (*neighbors[i]).getVelocity();
		}
		nextVelocity = nextVelocity / (float)neighbors.size();
	}

	return nextVelocity * m_relaxation;
};
#pragma once
#include "Law.hpp"
#include "../myMath/Vec2.hpp"
#include <vector>
#include "../../resources/model/Agent.hpp"
#include "../../resources/model/Flock.hpp"

class HuntingLaw : public Law {  // A changer
private:
	bool m_manualHunting;

public:
	HuntingLaw();
	HuntingLaw(Flock* pFlock);
	HuntingLaw(Flock* pFlock, bool manualHunting);
	Vec2 compute(Agent& currentAgent, const std::vector<Agent*>& neighbors) const override;
	Flock* m_pFlock;
};

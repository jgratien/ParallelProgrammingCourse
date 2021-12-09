#pragma once
#include "../../lib/myMath/Vec2.hpp"
#include "vector"
#include <tuple>
#include "Bird.hpp"


class Flock {
private :
	//std::vector<Bird> m_birdsVec;
	std::vector<Agent*> m_agents;
	std::vector<std::tuple<float, float, int>> m_x;
	std::vector<std::tuple<float, float, int>> m_y;

public:
	bool optimized_computing = false;
	void print();
	int getPopSize() const;
	Agent* getAgent(int index);
	Agent* operator[](int index) { return m_agents.at(index); };
	std::tuple<std::vector<Agent*>, std::vector<Agent*>> computeNeighbors(const Agent& agent);
	std::tuple<std::vector<Agent*>, std::vector<Agent*>> computeNeighborsOrigin(const Agent& agent);

	void addAgent(Agent *a);
	void setAgentsToBeDestroyed(const Vec2& position, const int& destroyRadius);
	void destroyAgents();
	void destroyLastAgent();
	void updateAgents();

public:
	Flock(std::vector<Agent*> population);
	Flock();
	~Flock() {}; // Destructeur

	auto begin() { return m_agents.begin(); };
	auto end() { return m_agents.end(); };

};

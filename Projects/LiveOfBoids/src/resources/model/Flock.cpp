#include "Flock.hpp"
#include "Bird.hpp"
#include "Eagle.hpp"
#include "../../lib/myMath/utils.hpp"
#include "../../lib/myMath/Vec2.hpp"
#include "../../lib/myLaws/Law.hpp"
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <tuple>

struct {
  bool operator()(std::tuple<float, float, int> a, std::tuple<float, float, int> b) const { return std::get<0>(a) < std::get<0>(b); }
} customLessX;

struct {
  bool operator()(std::tuple<float, float, int> a, std::tuple<float, float, int> b) const { return std::get<1>(a) < std::get<1>(b); }
} customLessY;

struct {
  bool operator()(std::tuple<float, float, int> a, std::tuple<float, float, int> b) const { return std::get<2>(a) < std::get<2>(b); }
} customLessIndex;

template<class InputIt1, class InputIt2, class OutputIt, class Compare>
OutputIt set_intersection(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt d_first, Compare comp)
{
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            ++first1;
        } else {
            if (!comp(*first2, *first1)) {
                *d_first++ = *first1++;
            }
            ++first2;
        }
    }
    return d_first;
}



Flock::Flock(std::vector<Agent*> population) : m_agents(population) {
	int size = (int) m_agents.size();
	for (int i = 0; i<size; ++i){
		m_x.push_back(std::tuple<float, float, int>{0., 0., i});
		m_y.push_back(std::tuple<float, float, int>{0., 0., i});
	}
};


int Flock::getPopSize() const {
	return (int)m_agents.size();
};

Flock::Flock() {};

void Flock::addAgent(Agent *a) {
	m_agents.emplace_back(a);
};

void Flock::setAgentsToBeDestroyed(const Vec2& position, const int& destroyRadius) {
	for (auto& bird : m_agents) {
		bool destroyBool = ((*bird).getPosition() - position).norm() < destroyRadius;
		if (destroyBool) { (*bird).setDestruction(); };
	}
}

void Flock::destroyAgents() {
	auto garbageAgents = std::remove_if(m_agents.begin(), m_agents.end(),
		[](Agent* a) {
			// If distance < 1, destroy bird
			bool destroyBool = ((*a).getDestruction());
			if (destroyBool) { delete a; };
			return destroyBool; });

	m_agents.erase(garbageAgents, m_agents.end());
};

void Flock::destroyLastAgent() {
	Agent *ptr = getAgent(getPopSize() - 1);
	delete ptr;
	m_agents.pop_back();
};

std::tuple<std::vector<Agent*>, std::vector<Agent*>> Flock::computeNeighbors(const Agent& agent){
  Vec2 pos = agent.getPosition();
	int range = agent.getRange();
  float x = pos.x;
  float y = pos.y;
  int indexXInf = researchX(x - range, m_x);
  int indexXSup = researchX(x + range, m_x);
  int indexYInf = researchX(y - range, m_y);
  int indexYSup = researchX(y + range, m_y);
  std::vector<std::tuple<float, float, int>> vx;
  std::vector<std::tuple<float, float, int>> vy;
  std::vector<std::tuple<float, float, int>> potentialNeighbors;
  vx.insert(vx.end(), m_x.begin() + indexXInf, m_x.begin() + indexXSup + 1);
  vy.insert(vy.end(), m_y.begin() + indexYInf, m_y.begin() + indexYSup + 1);
  std::sort(vx.begin(), vx.end(), customLessIndex);
  std::sort(vy.begin(), vy.end(), customLessIndex);
  std::set_intersection(
    vx.begin(), vx.end(),
    vy.begin(), vy.end(),
    std::back_inserter(potentialNeighbors),
    [](const auto & t1, const auto & t2) { return std::get<2>(t1) < std::get<2>(t2);}
  );
  float angle;
	std::vector<Agent*> neighbors;
	std::vector<Agent*> neighborsPredators;
	neighbors.reserve(potentialNeighbors.size());
  for (std::tuple<float, float, int> data : potentialNeighbors) {
		Agent * potentialNeighbor = m_agents[std::get<2>(data)];
    angle = degrees((pos - (*potentialNeighbor).getPosition()).angle());
		if (!(angle > agent.getViewAngle() * 0.5) && !(agent._id == (*potentialNeighbor)._id)){
			if (distance(pos, (*potentialNeighbor).getPosition()) <= range){
				if (dynamic_cast<Bird*> (potentialNeighbor) != nullptr) {
					neighbors.push_back(potentialNeighbor);
				}
				else if (dynamic_cast<Eagle*> (potentialNeighbor) != nullptr) {
					neighborsPredators.push_back(potentialNeighbor);
				}
			}
		}
	}
  return std::make_tuple( neighbors, neighborsPredators );
}


std::tuple<std::vector<Agent*>, std::vector<Agent*>> Flock::computeNeighborsOrigin(
			const Agent& agent){//, const float &range, const float &angle) {

	std::vector<Agent*> neighbors;
	std::vector<Agent*> neighborsPredators;
	neighbors.reserve(m_agents.size());

	//Like this one bird is going to be its own potential neighbor
	for (Agent* potentialNeighbor : m_agents) {
		if (!(degrees((agent.getPosition() - (*potentialNeighbor).getPosition()).angle()) > agent.getViewAngle() / 2) && !(agent._id == (*potentialNeighbor)._id))
		{
			double distWithPotNeighb = distance(agent.getPosition(), (*potentialNeighbor).getPosition());
			if (fabs(distWithPotNeighb) <= (float)abs(agent.getRange())) {
				if (dynamic_cast<Bird*> (potentialNeighbor) != nullptr) {
					neighbors.push_back(potentialNeighbor);
				}
				else if (dynamic_cast<Eagle*> (potentialNeighbor) != nullptr) {
					neighborsPredators.push_back(potentialNeighbor);
				}
			}
		}
	}
	return std::make_tuple( neighbors, neighborsPredators );
};

void Flock::updateAgents(){
	int size = (int) m_agents.size();
  float x;
  float y;
  m_x.clear();
  m_y.clear();
	for (int i = 0; i<size; ++i){
		Agent* agent = m_agents[i];
		Vec2 pos = (*agent).getPosition();
    x = pos.x;
    y = pos.y;
		m_x.push_back(std::tuple<float, float, int>{x, y, i});
		m_y.push_back(std::tuple<float, float, int>{x, y, i});
	}
	std::sort(m_x.begin(), m_x.end(), customLessX);
	std::sort(m_y.begin(), m_y.end(), customLessY);
}



void Flock::print() {
	std::cout << "Printing Flock :\n";
	int i = 0;
	for (Agent *b : m_agents) {
		std::cout << ++i << " : Pos(" << (*b).getPosition().x << ", " << (*b).getPosition().y << ")  //  Vel(" << (*b).getVelocity().x << ", " << (*b).getVelocity().y << ")" << std::endl;
	}
};

Agent* Flock::getAgent(int index) {
	return m_agents.at(index);
};

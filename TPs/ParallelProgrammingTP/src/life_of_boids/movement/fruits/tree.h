#pragma once
#include "eco.h"
#include "fruit.h"

class Tree : public Eco {
protected:
    double m_time; //Time for new fruit batch
public:
    Tree();
    Tree(Real const& x, Real const& y, Real const& size, double const& time);

    bool equal(Tree const& tree) const;
    bool overlap(std::vector<Tree> const& trees) const;

    void DropFruitAndAppend(std::vector<Fruit>& fruits);

    static std::vector<Tree> init(std::vector<Obstacle> const& obstacles);
    
    ~Tree();
};

#include "tree.h"

Tree::Tree() : Eco(), m_time(0) {}

Tree::Tree(Real const& x, Real const& y, Real const& size, double const& time) : Eco(x,y,size), m_time(time) {}

bool Tree::equal(Tree const& tree) const {
    return (m_x == tree.get_x() && m_y == tree.get_y());
}


bool Tree::overlap(std::vector<Tree> const& trees) const {

    auto check = [this](Tree const& tree) {
        return (!this->equal(tree) && this->distance(tree) < 2 * (m_size + tree.get_size()));
    };
    
    return std::any_of(trees.begin(), trees.end(), check);
}

void Tree::DropFruitAndAppend(std::vector<Fruit>& fruits) {

    std::uniform_real_distribution<Real> unif(0, 1); // Uniform distribution on [0:1] => Random number between 0 and 1
    std::uniform_real_distribution<Real> uniNumFruit(1, DEFAULT_NUM_FRUITS_DROPS);
    std::uniform_real_distribution<Real> uniTime(FRUIT_TIME_MIN, FRUIT_TIME_MAX);
    std::random_device dev;
    std::mt19937 engine(dev());
    time_t finish;

    Fruit fruit;

    if (m_time<=(double)time(&finish)) {
        int randomNumFruit;
        Real randomX;
        Real randomY;

        randomNumFruit = (int)uniNumFruit(engine);

        Real theta = 0;

        for (int i(0); i < randomNumFruit; i++) {
            randomX = m_x + 1.25 * m_size * cos(theta);
            randomY = m_y + 1.25 * m_size *  sin(theta);

            fruit = Fruit(randomX, randomY);
            fruits.emplace_back(randomX, randomY, MAX_FRUIT_TREE_SIZE/4, true);

            theta += 2*PI/randomNumFruit;
        }

/*        for (int i(0); i < randomNumFruit; i++) {
            std::uniform_real_distribution<Real> uniX(0, MAX_FRUIT_DISTANCE);
            std::uniform_real_distribution<Real> uniY(0, MAX_FRUIT_DISTANCE);
            randomX = m_x + uniX(engine);
            randomY = m_y + uniY(engine);
            fruit = Fruit(randomX, randomY);
            fruit.closestObstacle(obstacles);
            while (fruit.nearBorder() || fruit.get_obstacle()) {
                randomX = m_x + uniX(engine);
                randomY = m_y + uniY(engine);
                fruit = Fruit(randomX, randomY);
                fruit.closestObstacle(obstacles);
            }
            fruits.emplace_back(randomX, randomY, MAX_FRUIT_TREE_SIZE/4, true);
        }*/

        m_time = (double)uniTime(engine) + (double)time(&finish);
    }
}


Tree::~Tree() = default;

std::vector<Tree> Tree::init(std::vector<Obstacle> const& obstacles) {

    std::vector<Tree> trees;
    Tree newTree;

    Real randomX;
    Real randomY;
    int randomSize;
    double randomTime;

    std::uniform_real_distribution<Real> unif(0, 1); // Uniform distribution on [0:1] => Random number between 0 and 1
    std::uniform_real_distribution<Real> uniX(0, (Real)WIDTH);
    std::uniform_real_distribution<Real> uniY(0, (Real)HEIGHT);
    std::uniform_real_distribution<Real> uniSize(MIN_FRUIT_TREE_SIZE, MAX_FRUIT_TREE_SIZE);
    std::uniform_real_distribution<Real> uniTime(FRUIT_TIME_MIN, FRUIT_TIME_MAX);
    std::random_device dev;
    std::mt19937 engine(dev());
    time_t start;

    while (trees.size() < DEFAULT_NUM_TREES) {
        randomX = uniX(engine);
        randomY = uniY(engine);
        randomSize = (int)uniSize(engine);
        randomTime = (double)uniTime(engine)+(double)time(&start);
        newTree = Tree((Real)randomX, (Real)randomY, (Real)randomSize, randomTime);

        newTree.closestObstacle(obstacles);
        while (newTree.nearBorder() || newTree.get_obstacle() || newTree.overlap(trees)) {
            randomX = uniX(engine);
            randomY = uniY(engine);
            randomSize = uniSize(engine);
            newTree = Tree((Real)randomX, (Real)randomY, (Real)randomSize, randomTime);
            newTree.closestObstacle(obstacles);
        }

        trees.push_back(newTree);
    }
    return trees;
}
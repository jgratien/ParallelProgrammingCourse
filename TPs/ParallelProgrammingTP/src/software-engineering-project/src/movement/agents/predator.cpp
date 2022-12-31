#include "bird.h"
#include "predator.h"

Predator::Predator() : Agent() {}
Predator::Predator(Real const& x, Real const& y) : Agent(x, y) {}
Predator::Predator(Real const& x, Real const& y, Real const& angle) : Agent(x, y, angle) {}
Predator::Predator(Real const& x, Real const& y, Real const& angle, size_t& index) : Agent(x, y, angle, index) {}

std::vector<Real> Predator::neighbours(birds_t& birds, predators_t& predators) {
    std::vector<Real> v;

    Real current_distance;
    Real min_distance = (Real)(WIDTH + HEIGHT);

    for (auto& it : predators) {
    
        Predator& p = it.second;
        size_t p_index = p.get_index();
        current_distance = this->distance(p);

        if (m_index != p_index && (current_distance < PREDATOR_RANGE) && (current_distance < min_distance)) {

            if (this->insideFieldView(p)) {
                m_state = state::separation;
                min_distance = current_distance;
                v = { p.get_x(), p.get_y() };
            }
        }
    }

    if (m_state != state::separation) {

        min_distance = (Real)(WIDTH + HEIGHT);
        for (auto& it : birds) {

            Bird& b = it.second;
            current_distance = this->distance(b);

            if (current_distance < min_distance) {

                if (this->insideFieldView(b)) {
                    m_state = state::near_prey;
                    min_distance = current_distance;
                    v = { b.get_x(), b.get_y() };
                }
            }
        }
    }

    return v;
}


void Predator::preyLaw(std::vector<Real> const& bird) {

    vec2 target = calc::normVector({ (Real)(bird[0] - m_x),(Real)(bird[1] - m_y) });
    m_angle = calc::angle(PREDATOR_RELAXATION, target[1], target[0], m_angle);
}

void Predator::update(std::vector<Obstacle>const& obstacles, predators_t& predators, birds_t& birds) {
    
    std::vector<Real> closest_obstacle, closest_bird;
    m_state = state::constant;
    // Obstacles
    closest_obstacle = this->closestObstacle(obstacles);

    if (m_state == state::near_obstacle) {
        this->obstacleLaw(closest_obstacle);
    }
    else {
        // Neighbours and preys
        closest_bird = this->neighbours(birds, predators);

        // choose law by state and update the angle of agent
        switch (m_state) {

            case state::near_prey:
                this->preyLaw(closest_bird);
                break;
            case state::separation:
                this->separationLaw(closest_bird);
                break;
            default:
                break;
        }
    }

    // update the window and then set new x, y
    this->windowUpdate(PRED_SPEED);
}

Predator::~Predator() = default;

predators_t Predator::init(std::vector<Obstacle> const& obstacles) {

    predators_t predators;
    birds_t birds;
    Predator predator;

    int randomX;
    int randomY;
    Real randomAngle;
    size_t n = predators.size();

    std::uniform_real_distribution<Real> unif(0, 1); // Uniform distribution on [0:1] => Random number between 0 and 1
    std::uniform_real_distribution<Real> uniX(0, WIDTH);
    std::uniform_real_distribution<Real> uniY(0, HEIGHT);
    std::random_device dev;
    std::mt19937 engine(dev());

    for (size_t i = 0; i < DEFAULT_NUM_PREDATORS; ++i) {

        randomX = uniX(engine);
        randomY = uniY(engine);
        randomAngle = (Real)(2 * PI * unif(engine) - PI);

        predator = Predator((Real)randomX, (Real)randomY, randomAngle, n);
        predator.closestObstacle(obstacles);
        while (predator.get_state() == state::near_obstacle || predator.overlap(birds, predators)) {
            randomX = uniX(engine);
            randomY = uniY(engine);
            randomAngle = (Real)2 * PI * unif(engine) - PI;

            predator = Predator((Real)randomX, (Real)randomY, randomAngle, n);
            predator.closestObstacle(obstacles);
        }
        predators[n] = predator;
        n = predators.size();
    }

    return predators;
}

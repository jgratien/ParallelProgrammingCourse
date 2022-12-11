#include <algorithm>
#include "bird.h"
#include "predator.h"

Bird::Bird() : Agent(), m_alive(true) {}
Bird::Bird(Real const& x, Real const& y) : Agent(x, y), m_alive(true) {}
Bird::Bird(Real const& x, Real const& y, Real const& angle) : Agent(x,y,angle), m_alive(true) {}
Bird::Bird(Real const& x, Real const& y, Real const& angle, size_t& index) : Agent(x,y,angle,index), m_alive(true) {}

bool Bird::get_alive() const {
    return m_alive;
}

std::vector<Real> Bird::neighbours(birds_t& birds) {
    std::vector<Real> v;
    Real xSeparation(0), ySeparation(0);

    Real angleAlignment(0);
    int nAlignment(0);

    Real xCohesion(0), yCohesion(0);
    int nCohesion(0);

    Real current_distance;
    Real min_distance = (Real)(WIDTH+HEIGHT);
    Real inv;

    size_t n = birds.size();
    for (size_t i=0; i < n; i++) {
        Bird const& b = birds[i];
        if (m_index != i) {
            current_distance = this->distance(b);
            if ((current_distance < SEPARATION_RANGE) && (current_distance < min_distance)) {

                if (this->insideFieldView(b)) {
                    if (m_state == state::near_predator)
                        m_state = state::near_predatorANDseparation;
                    else if (m_state == state::near_fruit)
                        m_state = state::near_fruitANDseparation;
                    else
                        m_state = state::separation;
                    min_distance = current_distance;
                    xSeparation = b.get_x();
                    ySeparation = b.get_y();
                }
            }
            else if (((m_state == state::alignment) || (m_state == state::constant)) && (current_distance > SEPARATION_RANGE) && (current_distance < ALIGNMENT_RANGE)) {

                if (this->insideFieldView(b)) {
                    m_state = state::alignment;
                    angleAlignment += b.get_angle();
                    nAlignment++;
                }
            }
            else if (((m_state == state::cohesion) || (m_state == state::constant)) && (current_distance > ALIGNMENT_RANGE) && (current_distance < COHESION_RANGE)) {

                if (this->insideFieldView(b)) {
                    m_state = state::cohesion;
                    xCohesion += b.get_x();
                    yCohesion += b.get_y();
                    nCohesion++;
                }
            }
        }
    }
    
    // choose x, y by state
    switch (m_state) {
        case state::separation:
            v = {xSeparation,ySeparation};
            break;
        case state::near_predatorANDseparation:
            v = {xSeparation,ySeparation};
            break;
        case state::alignment:
            inv = (Real) 1 / (Real)(nAlignment + 1);
            v = {angleAlignment*inv};
            break;
        case state::cohesion:
            inv = (Real) 1 / (Real)(nCohesion + 1);
            v = {xCohesion*inv,yCohesion*inv};
            break;
        default:
            v = {0};
            break;
    }

    return v;
}
std::vector<Real> Bird::closestPredator(predators_t& predators) {
    std::vector<Real> pred;

    Real current_distance;
    Real min_distance = (Real)(WIDTH+HEIGHT);

    for (auto &it : predators) {
        
        Predator& p = it.second;
        current_distance = this->distance(p);

        if ((current_distance < PREDATOR_RANGE) && (current_distance < min_distance) ) {

            if (this->insideFieldView(p)) {
                m_state = state::near_predator;
                min_distance = current_distance;
                pred={p.get_x(), p.get_y()};

                if (current_distance < DEAD_RANGE) {
                    m_alive = false;
                    break;
                }
            }
        }
    }

    return pred;
}

std::vector<Real> Bird::closestFruit(std::vector<Fruit>& fruits) {

    std::vector<Real> fr;

    Real current_distance;
    Real min_distance = (Real)(WIDTH+HEIGHT);

    for (auto& f : fruits) {
        // some other thread might make it false
        if (!f.get_alive())
            continue;
        current_distance = this->distance(f);

        if ( (current_distance < FRUIT_RANGE) && (current_distance<min_distance) ) {
            m_state = state::near_fruit;
            min_distance = current_distance;
            fr = {f.get_x(),f.get_y()};

            if (current_distance < DEAD_RANGE) {
                f.get_alive() = false;
            }
        }
    }
    return fr;
}

void Bird::cohesionLaw(std::vector<Real> const& group) {

    m_angle = calc::angle(COHESION_RELAXATION, group[1] - m_y, group[0] - m_x, m_angle);
}

void Bird::alignmentLaw(std::vector<Real> const& group) {
    
    m_angle = (1-ALIGNMENT_RELAXATION)*group[0] + ALIGNMENT_RELAXATION*m_angle ;
}

void Bird::biSeparationLaw(std::vector<Real> const& bird, std::vector<Real> const& predator) {
    vec2 separationBird = calc::normVector({(Real)(m_x-bird[0]),(Real)(m_y-bird[1])});
    vec2 separationPred = calc::normVector({(Real)(m_x-predator[0]),(Real)(m_y-predator[0])});

    Real angleBird = calc::angle(SEPARATION_RELAXATION, separationBird[1], separationBird[0], m_angle);
    Real anglePred = calc::angle(SEPARATION_RELAXATION, separationPred[1], separationPred[0], m_angle);

    m_angle = (Real)(0.5*anglePred + 0.5*angleBird);
}

void Bird::fruitLaw(std::vector<Real> const& f) {

    vec2 target = calc::normVector({ (float)(f[0] - m_x),(float)(f[1] - m_y) });
    m_angle = calc::angle(PREDATOR_RELAXATION, target[1], target[0], m_angle);
}

void Bird::biFruitLaw(std::vector<Real> const& f, std::vector<Real> const& bird) {

    vec2 separationBird = calc::normVector({(Real)(m_x-bird[0]),(Real)(m_y-bird[1])});
    vec2 target = calc::normVector({ (float)(f[0] - m_x),(float)(f[1] - m_y) });

    Real angleBird = calc::angle(SEPARATION_RELAXATION, separationBird[1], separationBird[0], m_angle);
    Real angleFruit = calc::angle(PREDATOR_RELAXATION, target[1], target[0], m_angle);
   
    m_angle = (Real)(0.5*angleFruit + 0.5*angleBird);
}

bool Bird::update(std::vector<Obstacle>const& obstacles, predators_t& predators, birds_t& birds, std::vector<Fruit>& fruits) {

    m_state = state::constant;
    std::vector<Real> closest_obstacle, closest_bird, closest_predator, closest_fruit;
    // Predators
    closest_predator = this->closestPredator(predators);
    // already dead then return
    if (!m_alive) {
        return false;
    }

    // Obstacles
    closest_obstacle = this->closestObstacle(obstacles);

    if (m_state == state::near_obstacle) {
        this->obstacleLaw(closest_obstacle);
    }
    else {
        // Fruits
        if (m_state != state::near_predator)
            closest_fruit = this->closestFruit(fruits);

        // Neighbours
        closest_bird = this->neighbours(birds);

        // choose law by state and update the angle of agent
        switch (m_state) {
            case state::near_predatorANDseparation:
                this->biSeparationLaw(closest_bird, closest_predator);
                break;
            case state::near_predator:
                this->separationLaw(closest_predator);
                break;
            case state::near_fruitANDseparation:
                this->biFruitLaw(closest_fruit, closest_bird);
                break;
            case state::near_fruit:
                this->fruitLaw(closest_fruit);
                break;
            case state::separation:
                this->separationLaw(closest_bird);
                break;
            case state::alignment:
                this->alignmentLaw(closest_bird);
                break;
            case state::cohesion:
                this->cohesionLaw(closest_bird);
                break;
            default:
                break;
        }
    }
    // update the window and then set new x, y
    this->windowUpdate(SPEED);
    return true;
}

birds_t Bird::init(std::vector<Obstacle> const& obstacles, predators_t& predators) {

    birds_t birds;
    Bird bird;

    int randomX;
    int randomY;
    Real randomAngle;
    size_t n = birds.size();
    
    std::uniform_real_distribution<Real> unif(0, 1); // Uniform distribution on [0:1] => Random number between 0 and 1
    std::uniform_int_distribution<> uniX(0, WIDTH);
    std::uniform_int_distribution<> uniY(0, HEIGHT);
    std::random_device dev;
    std::mt19937 engine(dev());

    for (size_t i = 0; i<DEFAULT_NUM_BIRDS; ++i) {

        randomX = uniX(engine);
        randomY = uniY(engine);
        randomAngle = (Real)(2*PI*unif(engine)-PI);

        bird = Bird((Real)randomX,(Real)randomY,randomAngle,n);
        bird.closestObstacle(obstacles);
        while (bird.get_state() == state::near_obstacle || bird.overlap(birds, predators) ) {
            randomX = uniX(engine);
            randomY = uniY(engine);
            randomAngle = (Real)(2*PI*unif(engine)-PI);

            bird = Bird((Real)randomX,(Real)randomY,randomAngle,n);
            bird.closestObstacle(obstacles);
        }
        birds[n] = bird;
        n++;
    }

    return birds;
}

Bird::~Bird() = default;

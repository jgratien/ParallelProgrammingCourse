#include "../include/utils/utils.hpp"

#include <random>

float distance(const Vec2 a, const Vec2 b) {
    return (a - b).magnitude();
}

Vec2 barycenter(const std::vector<Vec2> vecs)
{
    int size = (int) vecs.size();
    float xg = 0;
    float yg = 0;
    for (Vec2 v : vecs)
    {
        xg += v.x;
        yg += v.y;
    }
    return Vec2(xg / size, yg / size);
}

float radians(const float angle) {
    return angle * 3.141592f / 180; 
}

float degrees(const float angle)
{
    return angle * 180 / 3.141592f;
}

bool isAntiClockwise(const Vec2 a, const Vec2 b)
{
    return (a.x * b.y - a.y * b.x >= 0);
}

float random_float(int inf, int sup)
{
    if (inf > sup) {
        throw std::invalid_argument("Lower bound must be less than upper bound");
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(static_cast<float>(inf), static_cast<float>(sup));
    return (float) dis(gen);
}

Vec2 randomVec2Generation(int infBoundary, int supBoundary)
{
    float x = random_float(infBoundary, supBoundary);
    float y = random_float(infBoundary, supBoundary);
    return Vec2(x, y);
}

Vec2 randomGenerationOnScreen(int width, int height, float ratio) {

    float x = random_float(0, ratio * width);
    float y = random_float(0, ratio * height);
    return Vec2(x, y);
}

Vec2 randomMovementGeneration(int infBoundary, int supBoundary) {
    std::random_device random_device;
    std::default_random_engine eng{random_device()};
    std::uniform_real_distribution<float> velocity_space{infBoundary, supBoundary};
    std::uniform_real_distribution<float> angle_space{0, 6.28};
    auto a = angle_space(eng);
    auto m = velocity_space(eng);
    return Vec2{m * std::cos(a), m * std::sin(a)};
}


   

int researchX(const float& target, const std::vector<std::tuple<float, float, int>>& v)
{
    long int size = (long int) v.size();
    if (std::get<0>(v[0]) >= target)
    {
        return 0;
    }
    else if (std::get<0>(v[(int) (size - 1.0)]) <= target)
    {
        return size - 1;
    }
    else
    {
        int d = 4;
        int a;
        int middle = (int) size / 2;
        while (!(std::get<0>(v[(int) (middle - 1.0)]) < target && target <= std::get<0>(v[middle])))
        {
            a = (int) size / d;
            if (target < std::get<0>(v[middle]))
            {
                middle -= (a != 0) ? a : 1;
            }
            else
            {
                middle += (a != 0) ? a : 1;
            }
            d *= 2;
        }
        return middle;
    }
}

int researchY(const float& target, const std::vector<std::tuple<float, float, int>>& v)
{
    int size = (int) v.size();
    if (std::get<1>(v[0]) > target)
    {
        return 0;
    }
    else if (std::get<1>(v[(int) (size - 1.0)]) <= target)
    {
        return size - 1;
    }
    else
    {
        int d = 4;
        int a;
        int middle = (int) size / 2;
        while (!(std::get<1>(v[(int) (middle - 1.0)]) <= target && target <= std::get<1>(v[middle])))
        {
            a = (int) size / d;
            if (target < std::get<1>(v[middle]))
            {
                middle -= (a != 0) ? a : 1;
            }
            else
            {
                middle += (a != 0) ? a : 1;
            }
            d *= 2;
        }
        return middle;
    }
}

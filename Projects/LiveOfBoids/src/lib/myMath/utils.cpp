#include "Vec2.hpp"
#include <vector>
#include <random>
#include "utils.hpp"
#include <tuple>

float distance(const Vec2 a, const Vec2 b) {
    return (a - b).norm();
}

Vec2 barycenter(const std::vector<Vec2> vecs) {
    int size = (int)vecs.size();
    float xg = 0;
    float yg = 0;
    for (Vec2 v : vecs) {
        xg += v.x;
        yg += v.y;
    }
    return Vec2(xg / size, yg / size);
}

float radians(const float angle) {
    return angle * 3.141592f / 180; //CHANGED M_PI because it causes error with gcc compiler, will try to add it again later
}

float degrees(const float angle) {
    return angle * 180 / 3.141592f;
}

bool isAntiClockwise(const Vec2 a, const Vec2 b){
  return (a.x * b.y - a.y * b.x >=0);
}

Vec2 randomVec2Generation(const int& infBoundary, const int& supBoundary)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(infBoundary, supBoundary);
    return Vec2((float)dis(gen), (float)dis(gen));
}

float random_float(const int &inf, const int &sup)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(inf, sup);
    return (float)dis(gen);
}


int researchX(const float& target, const std::vector<std::tuple<float, float, int>>& v){
  long int size = (long int)v.size();
  if (std::get<0>(v[0]) >= target){
    return 0;
  }
  else if (std::get<0>(v[(int)(size - 1.0)]) <= target) {
    return size - 1;
  }
  else{
    int d = 4;
    int a;
    int middle = (int) size / 2;
    while (!(std::get<0>(v[(int)(middle - 1.0)]) < target  && target <= std::get<0>(v[middle]))){
      a = (int)size / d;
      if (target < std::get<0>(v[middle])) {
        middle -= (a!=0) ? a : 1;
      }
      else {
        middle += (a!=0) ? a : 1;
      }
      d *= 2;
    }
    return middle;
  }
}

int researchY(const float& target, const std::vector<std::tuple<float, float, int>>& v){
  int size = (int) v.size();
  if (std::get<1>(v[0]) > target){
    return 0;
  }
  else if (std::get<1>(v[(int)(size - 1.0)]) <= target) {
    return size - 1;
  }
  else{
    int d = 4;
    int a;
    int middle = (int) size / 2;
    while (!(std::get<1>(v[(int)(middle - 1.0)]) <= target  && target <= std::get<1>(v[middle]))){
      a = (int) size / d;
      if (target < std::get<1>(v[middle])) {
        middle -= (a!=0) ? a : 1;
      }
      else {
        middle += (a!=0) ? a : 1;
      }
      d *= 2;
    }
    return middle;
  }
}

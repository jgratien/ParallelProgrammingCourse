#ifndef FREE_BIRDS_COLOR_HPP
#define FREE_BIRDS_COLOR_HPP

#include "MathUtils.hpp"

enum class Color { LightRed, White, Red, Green, Blue, Default, Yellow, Purple, Grey, Predator, Black, BlueGrey};

inline vec3 getColor(Color color){
    switch (color)
	{
	case Color::Red:
		return { 0.82745f, 0.184313f, 0.184313f };
	case Color::Green:
		return { 0.219607f, 0.55686f, 0.235294f };
	case Color::Blue:
		return { 0.09f, 0.99f, 0.99f };
	case Color::Yellow:
		return { 0.95f, 0.99f, 0.50f };
	case Color::Purple:
		return { 0.87f, 0.25f, 0.98f };
	case Color::Grey:
		return { 0.62f, 0.62f, 0.62f };
	case Color::Predator:
		return { 0.63f, 0.53f, 0.4980f };
	case Color::LightRed:
		return {0.8980f, 0.4509f, 0.4509f};
	default:
		return { 0.97f, 0.97f, 0.97f };
	}
}
#endif // COLOR_H
	
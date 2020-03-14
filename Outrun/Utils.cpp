#include "pch.h"
#include "Utils.h"

float Utils::Clamp(float value, float mn, float mx)
{
	if (value <= mn)
		value = mn;
	if (value >= mx)
		value = mx;

	return value;
}

float Utils::RandomFloat()
{
	// took this from stackoverflow: https://stackoverflow.com/questions/686353/random-float-number-generation
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

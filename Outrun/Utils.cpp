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

#pragma once

//#include <xnamath.h>
#include <cstdlib>

class MathHelper
{
public:
	// returns random float in [0, 1)
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	// returns random float in [a, b)
	static float RandF(const float a, const float b)
	{
		return a + RandF()*(b-a);
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > hight ? high : x);
	}

	static float AngleFromXY(const float x, const float y);
};

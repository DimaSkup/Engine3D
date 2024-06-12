#include "MathHelper.h"
#include <cmath>
#include <DirectXMath.h>

namespace ECS
{

float MathHelper::AngleFromXY(const float x, const float y)
{
	// get an angle in radians by the input x and y

	float theta = 0.0f;

	// quadrant I or IV
	if (x >= 0.0f)
	{
		// if x == 0, then atanf(y/x) = +pi/2 if y > 0
		//                 atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y/x);            // in [-pi/2, +pi/2]

		if (theta < 0.0f)
			theta += DirectX::XM_2PI;  // in [0, 2*pi).
	}

	// quadrant II or III
	else
		theta = atanf(y/x) + DirectX::XM_PI;

	return theta;
}

};  // namespace ECS
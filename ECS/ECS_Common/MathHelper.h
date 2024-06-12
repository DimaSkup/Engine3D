#pragma once

//#include <xnamath.h>
#include <cstdlib>
#include <DirectXMath.h>

namespace ECS
{

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

	static DirectX::XMFLOAT3 RandColorXMFLOAT3()
	{
		// returns a random color as XMFLOAT3 (RGB)

		const float red = MathHelper::RandF();   // random float in [0, 1)
		const float green = MathHelper::RandF();
		const float blue = MathHelper::RandF();

		return{ red, green, blue };
	}

	static DirectX::XMFLOAT4 RandColorXMFLOAT4()
	{
		// returns a random color as XMFLOAT4 (RGBA)

		const float red = MathHelper::RandF();   // random float in [0, 1)
		const float green = MathHelper::RandF();
		const float blue = MathHelper::RandF();

		return{ red, green, blue, 1.0f};
	}



	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x);
	}

	static float AngleFromXY(const float x, const float y);

	static const DirectX::XMMATRIX InverseTranspose(const DirectX::CXMMATRIX & M)
	{
		// if we apply a nonuniform scaling transformation A, the transformetd tangent vector
		// uA = v1*A - v0*A doesn't remain orthogonal to the transformed normal vector nA;
		// so the problem is this: given a transformation matrix A that transforms points and
		// vectors (non-normal), we want to find a transformation matrix B that transforms 
		// normal vectors such that the transformed tangent vector is orthogonal to the 
		// transformed normal vector (i.e., uA * nB = 0)

		DirectX::XMMATRIX A = M;

		// we clear out any translation from the matrix because we use the inverse-transpose
		// to transform vectors, and translations only apply to points
		A.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);

		DirectX::XMVECTOR det;// = DirectX::XMMatrixDeterminant(A);

		// return a transformation matrix B
		return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
	}
};

}; // namespace ECS
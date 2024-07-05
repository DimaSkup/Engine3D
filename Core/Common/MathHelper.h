#pragma once

//#include <xnamath.h>
#include <cstdlib>
#include <DirectXMath.h>


namespace DirectX
{
	static bool operator==(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
	{
		return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
	}

	static bool operator==(const XMFLOAT4& lhs, const XMFLOAT4& rhs)
	{
		return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z) && (lhs.w == rhs.w);
	}

	static bool operator==(const XMVECTOR& lhs, const XMVECTOR& rhs)
	{
		// define if two input XMVECTORs are equal

		const float* vec = XMVectorEqual(lhs, rhs).m128_f32;
		return (vec[0] && vec[1] && vec[2] && vec[3]);
	}

	static bool operator==(const XMMATRIX& lhs, const XMMATRIX& rhs)
	{
		// define if two input 4x4 matrices are equal

		return (lhs.r[0] == rhs.r[0]) &&
			(lhs.r[1] == rhs.r[1]) &&
			(lhs.r[2] == rhs.r[2]) &&
			(lhs.r[3] == rhs.r[3]);
	}
};

class MathHelper
{
public:
	static const float Infinity;
	static const float Pi;
	
	inline static float RandF()
	{
		// returns random float in [0, 1)
		return (float)(rand()) / (float)RAND_MAX;
	}

	inline static float RandF(const float a, const float b)
	{
		// returns random float in [a, b)
		return a + RandF()*(b-a);
	}

	inline static DirectX::XMFLOAT3 RandColorXMFLOAT3()
	{
		// returns a random color as XMFLOAT3 (RGB)
		return{ MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF() };
	}

	inline static DirectX::XMFLOAT4 RandColorXMFLOAT4()
	{
		// returns a random color as XMFLOAT4 (RGBA);
		// note: alpha value == 1.0f by default
		return{ MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF(), 1.0f};
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

//***************************************************************************************
// Waves.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************
#include "Waves.h"
#include <algorithm>
#include <cassert>

Waves::Waves()
{
}

Waves::~Waves()
{
}

//***************************************************************************************
//                             PUBLIC FUNCTIONS
//***************************************************************************************

UINT Waves::GetRowCount()      const { return numRows_; }
UINT Waves::GetColumnCount()   const { return numCols_; }
UINT Waves::GetVertexCount()   const { return vertexCount_; }
UINT Waves::GetTriangleCount() const { return triangleCount_; }

float Waves::GetWidth() const
{
	return numCols_ * spatialStep_;
}

float Waves::GetDepth() const
{
	return numRows_ * spatialStep_;
}

///////////////////////////////////////////////////////////

void Waves::Init(
	const UINT m,
	const UINT n,
	const float dx,
	const float dt,
	const float speed,
	const float damping)
{
	// initialize members with the waves params
	// and generate grid vertices of the waves

	numRows_ = m;
	numCols_ = n;

	vertexCount_ = m*n;
	triangleCount_ = (m-1)*(n-1)*2;

	timeStep_ = dt;
	spatialStep_ = dx;

	const float d_inv = 1.0f / (damping*dt + 2.0f);
	const float e     = pow(speed, 2) * pow(dt, 2) / pow(dx, 2);
	K1_               = (damping*dt - 2.0f) * d_inv;
	K2_               = (4.0f - 8.0f*e) * d_inv;
	K3_               = (2.0f*e) * d_inv;

	const float halfWidth = 0.5f * (n-1)*dx;
	const float halfDepth = 0.5f * (m-1)*dx;


	// temporal arrays for generated data (later it will be moved into members of the class)
	const UINT vertexCount = vertexCount_;
	std::vector<DirectX::XMFLOAT3> tempPrevSolution(vertexCount);
	std::vector<DirectX::XMFLOAT3> tempCurrSolution(vertexCount);

	// generate grid vertices of the waves
	for (UINT i = 0; i < m; ++i)
	{
		const float z = halfDepth - i*dx;

		for (UINT j = 0; j < n; j++)
		{
			const float x = -halfWidth + j*dx;
			const UINT idx = i*n + j;

			tempPrevSolution[idx] = { x, 0.0f, z };
			tempCurrSolution[idx] = { x, 0.0f, z };
		}
	}

	// move grid vertices into members of the class
	prevSolution_.resize(vertexCount);
	currSolution_.resize(vertexCount);

	std::move(tempPrevSolution.begin(), tempPrevSolution.end(), prevSolution_.begin());
	std::move(tempCurrSolution.begin(), tempCurrSolution.end(), currSolution_.begin());

	// in case Init() called again
	normals_.clear();
	tangentX_.clear();

	// each vertex has the same normal vector and the same tangentX vector
	normals_.insert(normals_.end(), vertexCount, { 0.0f, 1.0f, 0.0f });
	tangentX_.insert(tangentX_.end(), vertexCount, { 1.0f, 0.0f, 0.0f });
}

///////////////////////////////////////////////////////////

void Waves::Update(const float dt)
{
	static float t = 0;

	// accumulate time
	t += dt;

	// only update the simulation at the specified time step
	if (t >= timeStep_)
	{
		const UINT numRows = numRows_;
		const UINT numCols = numCols_;
		const float K[3] = { K1_, K2_, K3_ };

		// temporal arrays for vertices data
		std::vector<DirectX::XMFLOAT3> tPrevSolution(prevSolution_);
		std::vector<DirectX::XMFLOAT3> tCurrSolution(currSolution_);

		// only update interior points: we use zero boundary conditions
		for (UINT i = 1; i < numRows - 1; ++i)
		{
			const UINT i_mul_numCols = i*numCols;
			const UINT idx_1 = (i + 1)*numCols;
			const UINT idx_2 = (i - 1)*numCols;

			for (UINT j = 1; j < numCols - 1; ++j)
			{
				// after this update we will be discarding the old previous buffer,
				// so overwrite that buffer with the new update.
				// Note how we can do this inplace (read/write to same element)
				// because we won't need prev_ij again and the assignment happens last.

				// Note j indexes X and i indexes Z: h(x_j, z_i, t_k)
				// Moreover, our +Z axis goes "down"; this is just to  
				// keep consistent with our row indices going down
				const UINT idx = i_mul_numCols + j;

				tPrevSolution[idx].y = K[0]*tPrevSolution[idx].y +
				                       K[1]*tCurrSolution[idx].y +
					                   K[2]*tCurrSolution[idx_1+j].y +
					                        tCurrSolution[idx_2+j].y +
					                        tCurrSolution[idx+1].y +
					                        tCurrSolution[idx-1].y;
			}
		}

		// We just overwrote the previous buffer with the new data, so 
		// this data needs to become the current solution and the old 
		// current solution becomes the new previous solution
		prevSolution_ = tPrevSolution;
		std::swap(prevSolution_, currSolution_);

		// reset time
		t = 0.0f;

		//
		// Compute normals using finite difference scheme
		//
		for (UINT i = 1; i < numRows-1; ++i)
		{
			const UINT i_mul_numCols = i*numCols;
			const UINT idx_1 = (i + 1)*numCols;
			const UINT idx_2 = (i - 1)*numCols;

			for (UINT j = 1; j < numCols-1; ++j)
			{

			}
		}
	}


	return;
}

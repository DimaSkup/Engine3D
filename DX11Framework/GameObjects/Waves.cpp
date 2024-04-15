//***************************************************************************************
// Waves.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************
#include "Waves.h"
#include <algorithm>
#include <cassert>

using namespace DirectX;

Waves::Waves()
{
}

Waves::~Waves()
{
}

//***************************************************************************************
//                             PUBLIC FUNCTIONS
//***************************************************************************************

const std::vector<DirectX::XMFLOAT3> & Waves::GetPositions() const
{
	return currSolution_;
}

const std::vector<DirectX::XMFLOAT3> & Waves::GetNormals() const
{
	return normals_;
}

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

	// precompute constants
	
	const float f1     = (speed*speed) * (dt*dt) / (dx*dx);
	const float f2 = 1.0f / (damping*dt + 2.0f);
	K1_ = (4.0f - 8.0f*f1) * f2;
	K2_ = (damping*dt - 2.0f) * f2;
	K3_ = 2.0f * f1 * f2;

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

bool Waves::Update(const float dt)
{
	// UPDATE the shape of the waves;
	//
	// Input: delta time since the last frame;
	// Return: "true" if the updating was executed and "false" in another case;

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
		//std::vector<DirectX::XMFLOAT3> tPrevSolution(prevSolution_);
		//std::vector<DirectX::XMFLOAT3> tCurrSolution(currSolution_);

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


				prevSolution_[idx].y = K[0]* prevSolution_[idx].y +
				                       K[1]* currSolution_[idx].y +
					                   K[2]* currSolution_[idx_1+j].y +
					                        currSolution_[idx_2+j].y +
					                        currSolution_[idx+1].y +
					                        currSolution_[idx-1].y;

			}
		}

		// We just overwrote the previous buffer with the new data, so 
		// this data needs to become the current solution and the old 
		// current solution becomes the new previous solution
	//	prevSolution_ = tPrevSolution;
		std::swap(prevSolution_, currSolution_);

		// reset time
		t = 0.0f;

		//
		// Compute normals using finite difference scheme
		//

		for (UINT i = 1; i < numRows-1; ++i)
		{
			const UINT i_mul_numCols = i*numCols;
			const UINT idx_1 = (i - 1)*numCols;
			const UINT idx_2 = (i + 1)*numCols;

			for (UINT j = 1; j < numCols-1; ++j)
			{
				const UINT idx = i_mul_numCols + j;
				const float l = currSolution_[idx - 1].y;
				const float r = currSolution_[idx + 1].y;
				const float t = currSolution_[idx_1 + j].y;
				const float b = currSolution_[idx_2 + j].y;

				// make new normal vector and normalize it
				const XMVECTOR N = XMVector3Normalize({-r+1, 2.0f*spatialStep_, b-t});

				// make new tangentX vector and normalize it
				const XMVECTOR T = XMVector3Normalize({2.0f*spatialStep_, r-l, 0.0f});

				// store new normal and tangentX vectors
				XMStoreFloat3(&normals_[idx], N);
				XMStoreFloat3(&tangentX_[idx], T);
			}
		}

		return true;
	}


	return false;
}

///////////////////////////////////////////////////////////

void Waves::Disturb(const UINT i, const UINT j, const float magnitude)
{
	const UINT numCols = numCols_;

	// don't disturb boundaries
	assert(i > 1 && i < numRows_-2);
	assert(j > 1 && j < numCols-2);

	const float halfMag = 0.5f*magnitude;

	// disturb the ijth vertex height and its neighbors
	const UINT idx = i*numCols + j;
	currSolution_[idx].y             += magnitude;
	currSolution_[idx+1].y           += halfMag;
	currSolution_[idx-1].y           += halfMag;
	currSolution_[(i+1)*numCols+j].y += halfMag;
	currSolution_[(i-1)*numCols+j].y += halfMag;
}
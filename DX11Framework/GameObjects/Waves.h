//***************************************************************************************
// Waves.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Performs the calculations for the wave simulation.  After the simulation has been
// updated, the client must copy the current solution into vertex buffers for rendering.
// This class only does the calculations, it does not do any drawing.
//***************************************************************************************
#pragma once

#include <windows.h>
#include <DirectXMath.h>
#include <vector>

class Waves
{
public:
	Waves();
	~Waves();

	UINT GetRowCount() const;
	UINT GetColumnCount() const;
	UINT GetVertexCount() const;
	UINT GetTriangleCount() const;

	float GetWidth() const;
	float GetDepth() const;

	// returns the solution at the ith grid point
	const DirectX::XMFLOAT3 & operator[](int i) const { return currSolution_[i]; }

	void Init(const UINT m, const UINT n, const float dx, const float dt, const float speed, const float damping);
	void Update(const float dt);
	void Disturb(const UINT i, const UINT j, const float magnitude);

private:
	UINT numRows_ = 0;
	UINT numCols_ = 0;

	UINT vertexCount_ = 0;
	UINT triangleCount_ = 0;

	// simulation constants we can precompute
	float K1_ = 0.0f;
	float K2_ = 0.0f;
	float K3_ = 0.0f;

	float timeStep_ = 0.0f;
	float spatialStep_ = 0.0f;

	std::vector<DirectX::XMFLOAT3> prevSolution_;
	std::vector<DirectX::XMFLOAT3> currSolution_;
	std::vector<DirectX::XMFLOAT3> normals_;
	std::vector<DirectX::XMFLOAT3> tangentX_;
};
////////////////////////////////////////////////////////////////////
// Filename:     ModelMath.h
// Description:  constains functions for calculation model math
//               (normal, tangent, binormal, etc.)
//
// Created:      05.02.23
////////////////////////////////////////////////////////////////////
#pragma once

#include <DirectXMath.h>

#include <vector>

#include "Vertex.h"



//////////////////////////////////
// Class name: ModelMath
//////////////////////////////////
class ModelMath
{
public:
	// function for calculating the tangent and binormal vectors for the model
	void CalculateModelVectors(
		std::vector<Vertex3D>& verticesArr,
		const bool calculateNormals = true);

	void CalculateTangentBinormal(
		const Vertex3D& vertex1,
		const Vertex3D& vertex2,
		const Vertex3D& vertex3,
		DirectX::XMVECTOR& tangent,
		DirectX::XMVECTOR& binormal);

	void CalculateNormal(
		const DirectX::XMVECTOR& tangent,
		const DirectX::XMVECTOR& binormal,
		DirectX::XMVECTOR& normal);
};


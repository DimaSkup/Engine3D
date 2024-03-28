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
		_Inout_ std::vector<VERTEX> & verticesArr,
		const bool calculateNormals = true);

	void CalculateTangentBinormal(
		const VERTEX & vertex1,
		const VERTEX & vertex2,
		const VERTEX & vertex3,
		_Out_ DirectX::XMVECTOR & tangent,
		_Out_ DirectX::XMVECTOR & binormal);

	void CalculateNormal(
		const DirectX::XMVECTOR & tangent,
		const DirectX::XMVECTOR & binormal,
		_Out_ DirectX::XMVECTOR & normal);
};


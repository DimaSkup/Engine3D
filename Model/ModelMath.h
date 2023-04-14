////////////////////////////////////////////////////////////////////
// Filename:     ModelMath.h
// Description:  constains functions for calculation model math
//               (normal, tangent, binormal, etc.)
//
// Created:      05.02.23
////////////////////////////////////////////////////////////////////
#pragma once

#include <DirectXMath.h>
#include <cmath>

#include "Vertex.h"



//////////////////////////////////
// Class name: ModelMath
//////////////////////////////////
class ModelMath
{
protected:

	// internal representation of a model vertex structure
	// (must be the same both in this class and in the ModelClass as well)
	/*
	
	struct ModelType
	{
		ModelType() :
			position{ 0.0f, 0.0f, 0.0f },
			texture{ 0.0f, 0.0f },
			normal{ 0.0f, 0.0f, 0.0f },
			color{ 0.0f, 0.0f, 0.0f, 1.0f },
			tangent{ 0.0f, 0.0f, 0.0f },
			binormal{ 0.0f, 0.0f, 0.0f }
		{

		}

		DirectX::XMFLOAT3 position;   // position coords
		DirectX::XMFLOAT2 texture;    // texture coords
		DirectX::XMFLOAT3 normal;     // normal
		DirectX::XMFLOAT4 color;      // tangent
		DirectX::XMFLOAT3 tangent;    // binormal
		DirectX::XMFLOAT3 binormal;   // colour (RGBA)

	};
	*/

	// the following two structures will be used for calsulation the tangen and binormal
	struct TempVertexType
	{
		TempVertexType() :
			x(0), y(0), z(0),
			tu(0), tv(0), 
			nx(0), ny(0), nz(0) {}

		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};


public:
	// function for calculating the tangent and binormal vectors for the model
	void CalculateModelVectors(VERTEX* pModelTypeData, size_t vertexCount, bool calculateNormals = true);
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);
};


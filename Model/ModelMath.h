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
	// the following two structures will be used for calsulation the tangent and binormal
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


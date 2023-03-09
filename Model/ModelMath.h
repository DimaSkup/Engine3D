////////////////////////////////////////////////////////////////////
// Filename:     ModelMath.h
// Description:  constains functions for calculation model math
//               (normal, tangent, binormal, etc.)
//
// Created:      05.02.23
////////////////////////////////////////////////////////////////////
#pragma once

#include <cmath>



//////////////////////////////////
// Class name: ModelMath
//////////////////////////////////
class ModelMath
{
protected:

	// internal representation of a model vertex structure
	// (must be the same both in this class and in the ModelClass as well)
	struct ModelType
	{
		ModelType()
		{
			x = y = z = 0.0f;
			tu = tv = 0.0f;
			nx = ny = nz = 0.0f;
			tx = ty = tz = 0.0f;
			bx = by = bz = 0.0f;
			cr = cg = cb = ca = 1.0f;  // by default we set a white colour for each vertex
		}

		float x, y, z;         // position coords
		float tu, tv;          // texture coords
		float nx, ny, nz;      // normal
		float cr, cg, cb, ca;  // colour (RGBA)
		float tx, ty, tz;      // tangent
		float bx, by, bz;      // binormal

	};



	// the following two structures will be used for calsulation the tangen and binormal
	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};


public:
	// function for calculating the tangent and binormal vectors for the model
	void CalculateModelVectors(void* pModelTypeData, size_t vertexCount);
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);
};


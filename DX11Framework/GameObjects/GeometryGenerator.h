////////////////////////////////////////////////////////////////////////////////////////////
// Filename:    GeometryGenerator.h
// Description: this class is a utility class for generating simple geometric shapes
//              (for instance: grid, cylinder, sphere, box, etc.)
//
// Created:     13.03.24
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>
#include "Vertex.h"

typedef unsigned int UINT;

class GeometryGenerator final
{
private:
	struct MeshData
	{
		std::vector<VERTEX> vertices;
		std::vector<UINT> indices;
	};

public:
	GeometryGenerator();

	void CreateGrid(
		const float widht,
		const float depth,
		const UINT m,
		const UINT n,
		MeshData & meshData);
};
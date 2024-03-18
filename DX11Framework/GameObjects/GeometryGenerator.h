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
public:
	struct MeshData
	{
		std::vector<VERTEX> vertices;
		std::vector<UINT> indices;
	};

public:
	GeometryGenerator();

	void CreateAxis(MeshData & meshData);

	void CreateGrid(
		const float widht,
		const float depth,
		const UINT m,
		const UINT n,
		MeshData & meshData);

	void CreateCylinder(
		const float bottomRadius,
		const float topRadius,
		const float height,
		const UINT sliceCount,
		const UINT stackCount,
		MeshData & meshData);

	void CreateSphere(
		const float radius,
		const UINT sliceCount,
		const UINT stackCount,
		MeshData & sphereMesh);

private:
	void CreateCylinderStacks(
		const float bottomRadius,
		const float topRadius,
		const float height,
		const UINT sliceCount,
		const UINT stackCount,
		MeshData & meshData);

	void BuildCylinderTopCap(
		const float bottomRadius,
		const float topRadius,
		const float height,
		const UINT sliceCount,
		const UINT stackCount,
		MeshData & meshData);

	void BuildCylinderBottomCap(
		const float bottomRadius,
		const float topRadius,
		const float height,
		const float sliceCount,
		const float stackCount,
		MeshData & meshData);
};
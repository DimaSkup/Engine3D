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
#include "Waves.h"

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

	void CreateAxisMesh(MeshData & meshData);

	void CreateCubeMesh(MeshData & meshData);

	void CreateGridMesh(
		const float widht,
		const float depth,
		const UINT m,
		const UINT n,
		MeshData & meshData);

	void CreatePyramidMesh(
		const float height,
		const float baseWidth,
		const float baseDepth,
		_Out_ MeshData & meshData);

	void CreateWavesMesh(
		const UINT numRows,
		const UINT numColumns,
		const float spatialStep,
		const float timeStep,
		const float speed,
		const float damping,
		Waves & waves,
		_Out_ MeshData & wavesMesh);

	void CreateCylinderMesh(
		const float bottomRadius,
		const float topRadius,
		const float height,
		const UINT sliceCount,
		const UINT stackCount,
		MeshData & meshData);

	void CreateSphereMesh(
		const float radius,
		const UINT sliceCount,
		const UINT stackCount,
		MeshData & sphereMesh);

	void CreateGeosphereMesh(
		const float radius,
		UINT numSubdivisions,
		MeshData & meshData);

private:
	// Private cylinder creation API 
	// (CreateCylinderStacks, BuildCylinderTopCap, BuildCylinderBottomCap)
	void CreateCylinderStacks(
		const float bottomRadius,
		const float topRadius,
		const float height,
		const UINT sliceCount,
		const UINT stackCount,
		const std::vector<float> & tu,           // texture X coords
		const std::vector<float> & thetaSinuses, // precomputed values of sinuses for each Theta value
		const std::vector<float> & thetaCosines, // precomputed values of cosines for each Theta value
		MeshData & meshData);

	void BuildCylinderTopCap(
		const float topRadius,
		const float height,
		const UINT sliceCount,
		const std::vector<float> & thetaSinuses,
		const std::vector<float> & thetaCosines,
		MeshData & meshData);

	void BuildCylinderBottomCap(
		const float bottomRadius,
		const float height,
		const UINT sliceCount,
		const std::vector<float> & thetaSinuses,
		const std::vector<float> & thetaCosines,
		MeshData & meshData);

	void Subdivide(MeshData & outMeshData);
};
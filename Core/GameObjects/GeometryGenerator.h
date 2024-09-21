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
#include "MeshHelperTypes.h"

typedef unsigned int UINT;

class GeometryGenerator final
{

public:
	GeometryGenerator();

	void GenerateAxisMesh(Mesh::MeshData & meshData);
	void GenerateCubeMesh(Mesh::MeshData & meshData);

	void GeneratePlaneMesh(
		const float width,
		const float height,
		Mesh::MeshData& meshData);

	void GenerateFlatGridMesh(
		const float widht,
		const float depth,
		const UINT m,
		const UINT n,
		Mesh::MeshData & meshData);

	void GeneratePyramidMesh(
		const float height,
		const float baseWidth,
		const float baseDepth,
		Mesh::MeshData & meshData);

	void GenerateWavesMesh(
		const UINT numRows,
		const UINT numColumns,
		const float spatialStep,
		const float timeStep,
		const float speed,
		const float damping,
		Waves & waves,
		Mesh::MeshData & wavesMesh);

	void GenerateCylinderMesh(
		const float bottomRadius,
		const float topRadius,
		const float height,
		const UINT sliceCount,
		const UINT stackCount,
		Mesh::MeshData & meshData);

	void GenerateSphereMesh(
		const float radius,
		const UINT sliceCount,
		const UINT stackCount,
		Mesh::MeshData & sphereMesh);

	void GenerateGeosphereMesh(
		const float radius,
		UINT numSubdivisions,
		Mesh::MeshData & meshData);

private:
	// helper functions for a cube creation
	void SetupCubeVerticesPositions(std::vector<DirectX::XMFLOAT3>& verticesPos);
	void SetupCubeFacesNormals(std::vector<DirectX::XMFLOAT3>& facesNormals);

	// cylinder private creation API 
	// (BuildCylinderStacks, BuildCylinderTopCap, BuildCylinderBottomCap)
	void BuildCylinderStacks(
		const float bottomRadius,
		const float topRadius,
		const float height,
		const UINT sliceCount,
		const UINT stackCount,
		const std::vector<float> & tu,           // texture X coords
		const std::vector<float> & thetaSinuses, // precomputed values of sinuses for each Theta value
		const std::vector<float> & thetaCosines, // precomputed values of cosines for each Theta value
		Mesh::MeshData & meshData);

	void BuildCylinderTopCap(
		const float topRadius,
		const float height,
		const UINT sliceCount,
		const std::vector<float> & thetaSinuses,
		const std::vector<float> & thetaCosines,
		Mesh::MeshData & meshData);

	void BuildCylinderBottomCap(
		const float bottomRadius,
		const float height,
		const UINT sliceCount,
		const std::vector<float> & thetaSinuses,
		const std::vector<float> & thetaCosines,
		Mesh::MeshData & meshData);

	// helper function for a geosphere creation
	void Subdivide(Mesh::MeshData & outMeshData);

	void SetDefaultMaterial(Mesh::Material& mat);
};
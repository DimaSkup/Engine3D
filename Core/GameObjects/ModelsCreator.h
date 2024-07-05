////////////////////////////////////////////////////////////////////////////////////////////
// Filename:        ModelsCreator.h
// Description:     a functional for mesh/models creation
//                  (plane, cube, sphere, imported models, etc.)
//
// Created:         12.02.24
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>     
#include <fstream>
#include "MeshHelperTypes.h"
#include "MeshStorage.h"
#include "TextureManagerClass.h"

typedef unsigned int UINT;

class ModelsCreator
{
public:
	ModelsCreator();

	const std::vector<MeshID> ImportFromFile(
		ID3D11Device* pDevice, 
		const std::string& filepath);

	const std::vector<TextureClass*> GetDefaultTexturesSet() const;

	// create a model (mesh) according to its type and params
	MeshID Create(ID3D11Device* pDevice, const Mesh::MeshType& type);

	MeshID CreatePlane(ID3D11Device* pDevice);
	MeshID CreateCube(ID3D11Device* pDevice);
	MeshID CreateSkull(ID3D11Device* pDevice);
	MeshID CreatePyramid(ID3D11Device* pDevice, const Mesh::PyramidMeshParams params = NULL);
	MeshID CreateSphere(ID3D11Device* pDevice, const Mesh::SphereMeshParams params = NULL);
	MeshID CreateCylinder(ID3D11Device* pDevice, const Mesh::CylinderMeshParams params = NULL);

	MeshID CreateGrid(ID3D11Device* pDevice, const UINT width, const UINT depth);
	//MeshID CreateGeneratedTerrain(ID3D11Device* pDevice);
	
#if 0
	void GenerateHeightsForGrid(Mesh::MeshData & grid);

	void PaintGridWithRainbow(Mesh::MeshData & grid,
		const UINT verticesCountByX,
		const UINT verticesCountByZ);

	void PaintGridAccordingToHeights(Mesh::MeshData & grid);
#endif
};




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

	// create a model (mesh) according to its type and params
	static MeshID Create(
		const Mesh::MeshType& type,
		const Mesh::MeshGeometryParams& params, 
		ID3D11Device* pDevice);


private:
	const std::unordered_map<aiTextureType, TextureClass*> GetDefaultTexturesMap() const;

	MeshID CreatePlaneHelper(ID3D11Device* pDevice, const Mesh::MeshGeometryParams& params);
	MeshID CreateCubeHelper(ID3D11Device* pDevice, const Mesh::MeshGeometryParams& params);
	MeshID CreateSkullHelper(ID3D11Device* pDevice, const Mesh::MeshGeometryParams& params);
	MeshID CreatePyramidHelper(ID3D11Device* pDevice, const Mesh::MeshGeometryParams& params);
	MeshID CreateSphereHelper(ID3D11Device* pDevice, const Mesh::MeshGeometryParams& params);
	MeshID CreateCylinderHelper(ID3D11Device* pDevice, const Mesh::MeshGeometryParams& params);
	
	
#if 0
	void GenerateHeightsForGrid(Mesh::MeshData & grid);

	void PaintGridWithRainbow(Mesh::MeshData & grid,
		const UINT verticesCountByX,
		const UINT verticesCountByZ);

	void PaintGridAccordingToHeights(Mesh::MeshData & grid);
#endif
};




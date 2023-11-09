/////////////////////////////////////////////////////////////////////
// Filename:      ModelLoader.h
// Description:   contains a functional for loading model data (internal model type)
//                from a data file into a model object
//
// Created:       27.02.23
/////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>

#include "../Engine/Settings.h"
#include "../Engine/log.h"
#include "Vertex.h"

using namespace std;


//////////////////////////////////
// Class name: ModelLoader
//////////////////////////////////
class ModelLoader
{
public:
	ModelLoader();
	~ModelLoader();

	void Shutdown();

	// load model data from a data file
	// and write this data into the vertices/indices arrays
	bool Load(const std::string & filePath,
		std::vector<VERTEX> & verticesArr, 
		std::vector<UINT> & indicesArr);


	//
	// GETTERS
	//
	UINT GetVertexCount() const _NOEXCEPT;
	UINT GetIndexCount() const _NOEXCEPT;

private:
	bool LoadModelVITCount(ifstream & fin);    // load the number of vertices (V), indices (I), and textures (T) coordinates
	bool LoadModelVertexData(ifstream & fin, std::vector<VERTEX> & verticesArr);
	bool LoadModelIndexData(ifstream & fin, std::vector<UINT> & indicesArr);
	bool LoadModelTextureData(ifstream & fin);

	// initialize an internal model data structure
	bool InitializeInternalModelDataType(std::vector<VERTEX> & verticesArr);

private:
	//UINT* pVertexIndicesData_ = nullptr;
	UINT* pTextureIndicesData_ = nullptr;
	DirectX::XMFLOAT2* pTexturesData_ = nullptr;
	//DirectX::XMFLOAT3* pVerticesData_ = nullptr;

	UINT vertexCount_ = 0;
	UINT indexCount_ = 0;
	UINT texturesCount_ = 0;
};
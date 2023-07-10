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

	// load model data from a data file
	bool Load(std::string modelName, VERTEX** ppModelData, UINT** ppIndicesData);


	//
	// GETTERS
	//
	UINT GetVertexCount() const _NOEXCEPT;
	UINT GetIndexCount() const _NOEXCEPT;

private:
	bool LoadModelVITCount(ifstream & fin);    // load the number of vertices (V), indices (I), and textures (T) coordinates
	bool LoadModelVertexData(ifstream & fin);  
	bool LoadModelIndexData(ifstream & fin);
	bool LoadModelTextureData(ifstream & fin);

	// initialize an internal model data structure
	bool InitializeInternalModelDataType(VERTEX** ppModelData, UINT** ppIndicesData);   

private:
	UINT* pVertexIndicesData_ = nullptr;
	UINT* pTextureIndicesData_ = nullptr;
	DirectX::XMFLOAT2* pTexturesData_ = nullptr;
	DirectX::XMFLOAT3* pVerticesData_ = nullptr;

	UINT vertexCount_ = 0;
	UINT indexCount_ = 0;
	UINT texturesCount_ = 0;
};
///////////////////////////////////////////////////////////////////////////////////////////
// Filename:      ModelLoader.h
// Description:   contains a functional for loading model data (internal model type)
//                from a data file into a model object
//
// Created:       27.02.23
///////////////////////////////////////////////////////////////////////////////////////////
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
	UINT GetVertexCount() const;
	UINT GetIndexCount() const;

private:
	bool LoadModelVITCount(ifstream & fin);    // load the number of vertices (V), indices (I), and textures (T) coordinates
	bool LoadModelVertexData(ifstream & fin);
	bool LoadModelIndexData(ifstream & fin, std::vector<UINT> & indicesArr);
	bool LoadModelTextureData(ifstream & fin);

	// initialize an internal model data structure
	bool InitializeInternalModelDataType(std::vector<VERTEX> & verticesArr, std::vector<UINT> & indicesArr);

private:
	std::vector<DirectX::XMFLOAT3> verticesCoordsArr_;
	std::vector<DirectX::XMFLOAT2> texturesCoordsArr_;

	std::vector<UINT> textureIndicesArr_;
	

	UINT vertexCount_ = 0;
	UINT indexCount_ = 0;
	UINT texturesCount_ = 0;
};
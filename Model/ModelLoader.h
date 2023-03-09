#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "modelconverterclass.h"
#include "../Engine/log.h"
#include "Vertex.h"

using namespace std;

class ModelLoader
{
public:
	ModelLoader();
	~ModelLoader();


	bool Load(std::string modelName, VERTEX** ppModelData, UINT** ppIndicesData);
	UINT GetIndexCount() const;

private:
	bool LoadModelVITCount(ifstream & fin);
	bool LoadModelVertexData(ifstream & fin);
	bool LoadModelIndexData(ifstream & fin);
	bool LoadModelTextureData(ifstream & fin);

	bool InitializeInternalModelDataType(VERTEX** ppModelData, UINT** ppIndicesData);

private:
	size_t* pVertexIndicesData_ = nullptr;
	size_t* pTextureIndicesData_ = nullptr;
	DirectX::XMFLOAT2* pTexturesData_ = nullptr;
	DirectX::XMFLOAT3* pVerticesData_ = nullptr;
	const bool PRINT_DEBUG_DATA_ = false;

	UINT vertexCount_ = 0;
	UINT indexCount_ = 0;
	UINT texturesCount_ = 0;
};
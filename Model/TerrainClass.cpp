////////////////////////////////////////////////////////////////////
// Filename::    TerrainClass.cpp
// Description:  the terrain class will encapsulate the model data and 
//               rendering functionality for drawing the terrain (or terrain grid)
//
// Created:      11.03.23
////////////////////////////////////////////////////////////////////
#include "TerrainClass.h"



TerrainClass::TerrainClass()
{
}

TerrainClass::TerrainClass(const TerrainClass& copy)
{
}

TerrainClass::~TerrainClass()
{
}


////////////////////////////////////////////////////////////////////
//
//                     PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////

// the Inialize() function will just call the functions for initializing the 
// vertex and index buffers that will hold the terrain data
bool TerrainClass::Initialize(ID3D11Device* pDevice)
{
	bool result = false;
	ModelListClass* pModelList = ModelListClass::Get();

	this->CreateTerrainData();
	result = this->InitializeBuffers(pDevice);
	COM_ERROR_IF_FALSE(result, "can't intialize buffers for the terrain grid");

	this->SetID(modelType_);

	// print a message about the initialization process
	string debugMsg = modelType_ + " is initialized!";
	Log::Debug(THIS_FUNC, debugMsg.c_str());

	return true;
}


// Put the vertex buffer data and index buffer data on the video card 
// to prepare this data for rendering
void TerrainClass::Render(ID3D11DeviceContext* pDeviceContext)
{
	this->RenderBuffers(pDeviceContext);

	// the single difference here is that we render buffers using another type of the primitive topology;
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	pMediator_->Render(pDeviceContext);

	

	return;
}



////////////////////////////////////////////////////////////////////
//
//                    PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////
void TerrainClass::CreateTerrainData()
{
	pModelData_ = nullptr;
	pIndicesData_ = nullptr;
	int terrainWidth = 0;
	int terrainHeight = 0;
	int index = 0;                                      // intialize the index into the vertex and index arrays
	float positionX = 0.0f;
	float positionZ = 0.0f;
	DirectX::XMFLOAT4 color{ 1.0f, 1.0f, 1.0f, 1.0f };  // set the color of the terrain grid


	// set the height and width of the terrain grid
	terrainHeight = 256;
	terrainWidth = 256;

	
	// calculated the number of vertices in the terrain
	vertexCount_ = (terrainWidth - 1) * (terrainHeight - 1) * 8;

	// set the index count to the same as the vertex count
	indexCount_ = vertexCount_;

	// create the vertex array
	pModelData_ = new VERTEX[vertexCount_];
	COM_ERROR_IF_FALSE(pModelData_, "can't allocate the memory for a vertex array");

	// create the index array
	pIndicesData_ = new UINT[indexCount_];

	// load the vertex array and index array with data
	for (size_t j = 0; j < (terrainHeight - 1); j++)
	{
		for (size_t i = 0; i < (terrainWidth - 1); i++)
		{
			// Line 1 -- upper left
			positionX = static_cast<float>(i);
			positionZ = static_cast<float>(j + 1);

			pModelData_[index].position = { positionX, 0.0f, positionZ };
			pModelData_[index].color = color;
			pIndicesData_[index] = index;
			index++;

			// Line 1 -- upper right
			positionX = static_cast<float>(i + 1);
			positionZ = static_cast<float>(j + 1);

			pModelData_[index].position = { positionX, 0.0f, positionZ };
			pModelData_[index].color = color;
			pIndicesData_[index] = index;
			index++;

			// Line 2 -- upper right
			positionX = static_cast<float>(i + 1);
			positionZ = static_cast<float>(j + 1);

			pModelData_[index].position = { positionX, 0.0f, positionZ };
			pModelData_[index].color = color;
			pIndicesData_[index] = index;
			index++;

			// Line 2 -- bottom right
			positionX = static_cast<float>(i + 1);
			positionZ = static_cast<float>(j);

			pModelData_[index].position = { positionX, 0.0f, positionZ };
			pModelData_[index].color = color;
			pIndicesData_[index] = index;
			index++;

			// Line 3 -- bottom right
			positionX = static_cast<float>(i + 1);
			positionZ = static_cast<float>(j);

			pModelData_[index].position = { positionX, 0.0f, positionZ };
			pModelData_[index].color = color;
			pIndicesData_[index] = index;
			index++;

			// Line 3 -- bottom left
			positionX = static_cast<float>(i);
			positionZ = static_cast<float>(j);

			pModelData_[index].position = { positionX, 0.0f, positionZ };
			pModelData_[index].color = color;
			pIndicesData_[index] = index;
			index++;

			// Line 4 -- bottom left
			positionX = static_cast<float>(i);
			positionZ = static_cast<float>(j);

			pModelData_[index].position = { positionX, 0.0f, positionZ };
			pModelData_[index].color = color;
			pIndicesData_[index] = index;
			index++;

			// Line 4 -- upper left
			positionX = static_cast<float>(i);
			positionZ = static_cast<float>(j + 1);

			pModelData_[index].position = { positionX, 0.0f, positionZ };
			pModelData_[index].color = color;
			pIndicesData_[index] = index;
			index++;
		}
	}
}
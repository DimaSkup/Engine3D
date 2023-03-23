////////////////////////////////////////////////////////////////////
// Filename:     ModelData.h
// Description:  this class is a DTO for the model data
//
// Created:      28.02.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <string>

#include "../Engine/log.h"
#include "Vertex.h"





//////////////////////////////////
// Class name: ModelData
//////////////////////////////////
class ModelData
{
public:
	ModelData();
	ModelData(const ModelData & data);
	~ModelData();

	// memory allocation (we need it because we use DirectX::XM-objects)
	void* operator new(size_t i);
	void operator delete(void* p);

	// copy model's data from the original
	//void operator=(ModelData* data); 


	// GETTERS
	VERTEX* GetModelData();
	UINT*   GetIndicesData();
	VERTEX** GetAddressOfModelData();
	UINT** GetAddressOfIndicesData();

	const DirectX::XMMATRIX & GetWorldMatrix();         // returns a model's world matrix
	const std::string & GetModelType() const;
	const std::string & GetID() const;                  // returns an identifier of the model
	UINT GetVertexCount() const;
	UINT GetIndexCount() const;

	const DirectX::XMFLOAT3 & GetPosition() const;
	const DirectX::XMFLOAT3 & GetScale() const;
	const DirectX::XMFLOAT2 & GetRotation() const;
	const DirectX::XMFLOAT4 & GetColor() const;


	// SETTERS
	void SetModelData(const VERTEX* pModelData, UINT verticesData);
	void SetIndexData(const UINT* pIndicesData, UINT indicesData);

	void SetModelType(const std::string& modelFilename);
	void SetID(const std::string& modelID);

	void SetVertexCount(UINT vertexCount);
	void SetIndexCount(UINT indexCount);

	// modificators of the model
	void SetPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotation(float angleX, float angleY);
	void SetColor(float red, float green, float blue, float alpha);




	
	
protected:
	VERTEX* pModelData_ = nullptr;
	UINT* pIndicesData_ = nullptr;

	// model properties in the world
	DirectX::XMMATRIX modelWorldMatrix_;
	DirectX::XMFLOAT3 position_;        // position of the model in the world
	DirectX::XMFLOAT3 scale_;           // scale of the model
	DirectX::XMFLOAT2 radianAngle_;     // current angles of the model rotation (in radians)
	DirectX::XMFLOAT4 color_;           // color of the model

										// we need these variables because we use this data during model math calculations
	UINT vertexCount_ = 0;
	UINT indexCount_ = 0;

	std::string modelType_{ "" };
	std::string modelID_{ "" };
};
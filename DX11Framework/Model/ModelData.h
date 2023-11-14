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
#include <vector>

#include "../Engine/log.h"
#include "Vertex.h"



//////////////////////////////////
// Class name: ModelData
//////////////////////////////////
class ModelData final
{
public:
	ModelData();
	ModelData(const ModelData & data);
	~ModelData();

	// memory allocation (we need it because we use DirectX::XM-objects)
	void* operator new(std::size_t count);                              // a replaceable allocation function
	void* operator new(std::size_t count, const std::nothrow_t & tag);  // a replaceable non-throwing allocation function
	void* operator new(std::size_t count, void* ptr);                   // a non-allocating placement allocation function
	void operator delete(void* p) noexcept;

	// by the input value of vertices and indices allocates memory for a vertex and index array;
	// and setup the number of vertices and indices of this model;
	void AllocateVerticesAndIndicesArrays(UINT vertexCount, UINT indexCount);

	// release memory from the model vertices/indices data
	void Shutdown();

	// when we does some manipulations with model we have to compute a new world matrix for this model
	inline void ComputeWorldMatrix()
	{
		modelWorldMatrix_ = scalingMatrix_ * rotationMatrix_ * translationMatrix_;
	}

	//
	// GETTERS
	//
	const DirectX::XMMATRIX & GetWorldMatrix();       // returns a model's world matrix
	const std::string & GetPathToDataFile() const;    // returns a path to data file of this model's type
	const std::string & GetID()             const;    // returns a model's ID

	// getters for model's vertices/indices data
	//VERTEX*  GetVerticesData();
	//UINT*    GetIndicesData();
	//VERTEX** GetAddressOfVerticesData();
	//UINT**   GetAddressOfIndicesData();

	std::vector<VERTEX> & GetVertices();
	std::vector<UINT> & GetIndices();
	
	UINT     GetVertexCount()     const;
	UINT     GetIndexCount()      const;

	// getters for model's position/scale/rotation/color/etc.
	const DirectX::XMFLOAT3 & GetPosition() const;
	const DirectX::XMFLOAT3 & GetScale()    const;
	const DirectX::XMFLOAT3 & GetRotation() const;
	const DirectX::XMFLOAT4 & GetColor()    const;

	//
	// SETTERS
	//
	void CopyVerticesData(const VERTEX* pModelData, UINT vertexCount);
	void CopyIndicesData(const UINT* pIndicesData, UINT indexCount);

	void SetPathToDataFile(const std::string& modelFilename);
	void SetID(const std::string& modelID);

	void SetVertexCount(const UINT vertexCount);
	void SetIndexCount(const UINT indexCount);

	// modificators of the model
	void SetPosition(const float x, const float y, const float z);
	void SetPosition(const DirectX::XMFLOAT3 & position);
	void SetPosition(const DirectX::XMFLOAT4 & position);
	void AdjustPosition(const DirectX::XMFLOAT3 & translatePos);

	void SetScale(const float x, const float y, const float z);
	void SetRotation(const float radianX, const float radianY, const float radianZ);
	void SetRotationInDegrees(const float angleX, const float angleY, const float angleZ);

	void SetColor(float red, float green, float blue, float alpha);
	void SetColor(const DirectX::XMFLOAT4 & color);

	
protected:
	
	DirectX::XMMATRIX modelWorldMatrix_;
	//DirectX::XMMATRIX beginPositionMatrix_ = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX rotationMatrix_;  
	DirectX::XMMATRIX scalingMatrix_;
	DirectX::XMMATRIX translationMatrix_;

	// model properties in the world
	DirectX::XMFLOAT3 position_;        // position of the model in the world
	DirectX::XMFLOAT3 scale_;           // scale of the model
	DirectX::XMFLOAT3 radianAngle_;     // current angles of the model rotation (in radians)
	DirectX::XMFLOAT4 color_;           // color of the model

	//VERTEX* pVerticesData_ = nullptr;
	//UINT* pIndicesData_ = nullptr;

	std::vector<VERTEX> verticesCoordsArr_;
	std::vector<UINT> indicesArr_;

	// we need these variables because we use this data during model math calculations
	UINT vertexCount_ = 0;
	UINT indexCount_ = 0;



	std::string pathToDataFile_{ "no_PATH_TO_DATA_FILE" };
	std::string modelID_{ "no_ID" };
};

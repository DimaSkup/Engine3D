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

	//
	// GETTERS
	//
	const DirectX::XMMATRIX & GetWorldMatrix()    _NOEXCEPT;    // returns a model's world matrix
	const std::string & GetPathToDataFile() const _NOEXCEPT;    // returns a path to data file of this model's type
	const std::string & GetID()             const _NOEXCEPT;    // returns a model's ID

	// getters for model's vertices/indices data
	VERTEX*  GetVerticesData()          _NOEXCEPT;
	UINT*    GetIndicesData()           _NOEXCEPT;
	VERTEX** GetAddressOfVerticesData() _NOEXCEPT;
	UINT**   GetAddressOfIndicesData()  _NOEXCEPT;
	UINT     GetVertexCount()     const _NOEXCEPT;
	UINT     GetIndexCount()      const _NOEXCEPT;

	// getters for model's position/scale/rotation/color/etc.
	const DirectX::XMFLOAT3 & GetPosition() const _NOEXCEPT;
	const DirectX::XMFLOAT3 & GetScale()    const _NOEXCEPT;
	const DirectX::XMFLOAT3 & GetRotation() const _NOEXCEPT;
	const DirectX::XMFLOAT4 & GetColor()    const _NOEXCEPT;

	//
	// SETTERS
	//
	void CopyVerticesData(const VERTEX* pModelData, UINT vertexCount);
	void CopyIndicesData(const UINT* pIndicesData, UINT indexCount);

	void SetPathToDataFile(const std::string& modelFilename) _NOEXCEPT;
	void SetID(const std::string& modelID) _NOEXCEPT;

	void SetVertexCount(UINT vertexCount) _NOEXCEPT;
	void SetIndexCount(UINT indexCount)   _NOEXCEPT;

	// modificators of the model
	void SetPosition(const float x, const float y, const float z)  _NOEXCEPT;
	void SetPosition(const DirectX::XMFLOAT3 & position) _NOEXCEPT;
	void SetPosition(const DirectX::XMFLOAT4 & position) _NOEXCEPT;
	void AdjustPosition(const DirectX::XMFLOAT3 & translatePos);
	void SetScale(float x, float y, float z)     _NOEXCEPT;
	void SetRotation(float radianX, float radianY, float radianZ) _NOEXCEPT;
	void SetRotationInDegrees(float angleX, float angleY, float angleZ) _NOEXCEPT;
	void SetColor(float red, float green, float blue, float alpha) _NOEXCEPT;
	void SetColor(const DirectX::XMFLOAT4 & color) _NOEXCEPT;

	
protected:
	// model properties in the world
	DirectX::XMMATRIX modelWorldMatrix_;
	DirectX::XMFLOAT3 position_;        // position of the model in the world
	DirectX::XMFLOAT3 scale_;           // scale of the model
	DirectX::XMFLOAT3 radianAngle_;     // current angles of the model rotation (in radians)
	DirectX::XMFLOAT4 color_;           // color of the model

	VERTEX* pVerticesData_ = nullptr;
	UINT* pIndicesData_ = nullptr;

	// we need these variables because we use this data during model math calculations
	UINT vertexCount_ = 0;
	UINT indexCount_ = 0;

	std::string modelType_{ "" };
	std::string modelID_{ "" };
};

////////////////////////////////////////////////////////////////////
// Filename:     ModelData.cpp
//
// Created:      28.02.23
////////////////////////////////////////////////////////////////////
#include "ModelData.h"


ModelData::ModelData()
{
	modelWorldMatrix_ = DirectX::XMMatrixIdentity(); // by default we set the model at the beginning of the world

	// setup the model's default position and scale
	position_ = { 0.0f, 0.0f, 0.0f };
	scale_ = { 1.0f, 1.0f, 1.0f };
	radianAngle_ = { 0.0f, 0.0f };
}

ModelData::ModelData(const ModelData & data)
{
}

ModelData::~ModelData()
{
	this->Shutdown();
}






/////////////////////////////////////////////////////////////////////////////////////////
// 
//                              PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////



// by the input value of vertices and indices allocates memory for a vertex and index array
// and setup the number of vertices and indices of this model
void ModelData::AllocateVerticesAndIndicesArrays(UINT vertexCount, UINT indexCount)
{
	try
	{
		// allocate memory for the arrays
		pVerticesData_ = new VERTEX[vertexCount];
		pIndicesData_ = new UINT[indexCount];
	}
	catch (std::bad_alloc & e)
	{
		_DELETE(pVerticesData_);
		_DELETE(pIndicesData_);

		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the vertex/index array");
	}


	// set that this model will have such number of vertices and indices
	vertexCount_ = vertexCount;
	indexCount_ = indexCount;

	return;
}



// release memory from the model vertices/indices data
void ModelData::Shutdown()
{
	_DELETE_ARR(pVerticesData_);
	_DELETE_ARR(pIndicesData_);

	return;
}


// memory allocation (we need it because we use DirectX::XM-objects)
void* ModelData::operator new(size_t i)
{
	if (void* ptr = _aligned_malloc(i, 16))
	{
		return ptr;
	}
	
	Log::Error(THIS_FUNC, "can't allocate the memory for object");
	throw std::bad_alloc{};
}

void ModelData::operator delete(void* p) noexcept
{
	_aligned_free(p);
}



/////////////////////////////////////////////////////////////////////////////////////////
// 
//                           PUBLIC FUNCTIONS: GETTERS
//
/////////////////////////////////////////////////////////////////////////////////////////

// returns a model world matrix
const DirectX::XMMATRIX & ModelData::GetWorldMatrix() _NOEXCEPT
{
	DirectX::XMMATRIX beginPosition = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(scale_.x, scale_.y, scale_.z);
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(radianAngle_.y, radianAngle_.x, 0.0f);
	DirectX::XMMATRIX translate = DirectX::XMMatrixTranslation(position_.x, position_.y, position_.z);

	modelWorldMatrix_ = beginPosition * rotation * scale * translate;

	return modelWorldMatrix_;
}

// returns a path to data file of this model's type
const std::string & ModelData::GetPathToDataFile() const  _NOEXCEPT
{
	return modelType_;
}

// returns an identifier of the model
const std::string & ModelData::GetID() const _NOEXCEPT
{
	return modelID_;
}


//
// GETTERS for model's vertices/indices data
//

// get a pointer to the model's vertices data array
VERTEX* ModelData::GetVerticesData() _NOEXCEPT
{
	return pVerticesData_;
}

// get a pointer to the model's indices data array
UINT* ModelData::GetIndicesData() _NOEXCEPT
{
	return pIndicesData_;
}

VERTEX** ModelData::GetAddressOfVerticesData() _NOEXCEPT
{
	return &pVerticesData_;
}

UINT** ModelData::GetAddressOfIndicesData() _NOEXCEPT
{
	return &pIndicesData_;
}

// Get the number of vertices
UINT ModelData::GetVertexCount(void) const _NOEXCEPT
{
	return vertexCount_;
}

// Get the number of indices
UINT ModelData::GetIndexCount(void) const _NOEXCEPT
{
	return indexCount_;
}


//
// GETTERS for model's position/scale/rotation/color/etc.
//
const DirectX::XMFLOAT3 & ModelData::GetPosition() const _NOEXCEPT { return position_; }
const DirectX::XMFLOAT3 & ModelData::GetScale()    const  _NOEXCEPT { return scale_; }
const DirectX::XMFLOAT2 & ModelData::GetRotation() const _NOEXCEPT { return radianAngle_; }
const DirectX::XMFLOAT4 & ModelData::GetColor()    const _NOEXCEPT { return color_; };







/////////////////////////////////////////////////////////////////////////////////////////
// 
//                           PUBLIC FUNCTIONS: SETTERS
//
/////////////////////////////////////////////////////////////////////////////////////////


void ModelData::CopyVerticesData(const VERTEX* pVertexData, UINT verticesCount)
{
	assert(pVertexData != nullptr);

	try
	{
		// before allocation of memory we have to release the used memory (if we have it)
		_DELETE_ARR(this->pVerticesData_);

		// try to allocate memory for vertices
		this->pVerticesData_ = new VERTEX[verticesCount];

		// copy each vertex data into the current model vertices array
		for (size_t i = 0; i < verticesCount; i++)
		{
			this->pVerticesData_[i] = pVertexData[i];
		}
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the model vertex data");
	}
	
	return;
}


void ModelData::CopyIndicesData(const UINT* pIndicesData, UINT indicesCount)
{
	assert(pIndicesData != nullptr);

	try
	{
		// before allocation of memory we have to release the used memory (if we have it)
		_DELETE_ARR(this->pIndicesData_);

		// try to allocate memory for indices
		this->pIndicesData_ = new UINT[indicesCount];

		// copy each index data into the current model indices array
		for (size_t i = 0; i < indicesCount; i++)
		{
			this->pIndicesData_[i] = pIndicesData[i];
		}
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the model index data");
	}

	return;
}


// setup a path to data file of this model's type (for example: /default/cube.txt)
void ModelData::SetPathToDataFile(const std::string& modelType) _NOEXCEPT
{
	this->modelType_ = modelType;
}

// set an identifier of the model
void ModelData::SetID(const std::string& modelId) _NOEXCEPT
{
	modelID_ = modelId;
}


void ModelData::SetVertexCount(UINT vertexCount) _NOEXCEPT
{
	this->vertexCount_ = vertexCount;
	SetIndexCount(vertexCount);   // currently we must have the same count of vertices and indices
}


void ModelData::SetIndexCount(UINT indexCount) _NOEXCEPT
{
	this->indexCount_ = indexCount;
}



// set model's position in the world
void ModelData::SetPosition(float x, float y, float z) _NOEXCEPT
{
	position_.x = x;
	position_.y = y;
	position_.z = z;

	return;
}

// set model's scaling
void ModelData::SetScale(float x, float y, float z) _NOEXCEPT
{
	scale_.x = x;
	scale_.y = y;
	scale_.z = z;

	return;
}

// set model's rotation
void ModelData::SetRotation(float radiansX, float radiansY) _NOEXCEPT
{
	radianAngle_.x = radiansX;
	radianAngle_.y = radiansY;

	return;
}

// set model's color
void ModelData::SetColor(float red, float green, float blue, float alpha) _NOEXCEPT
{
	color_.x = red;
	color_.y = green;
	color_.z = blue;
	color_.w = alpha;
}

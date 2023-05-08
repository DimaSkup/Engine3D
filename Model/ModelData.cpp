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


void ModelData::AllocateVerticesAndIndicesArray(UINT vertexCount, UINT indexCount)
{
	assert(vertexCount > 0);
	assert(indexCount > 0);

	try
	{
		pVerticesData_ = new VERTEX[vertexCount];
		pIndicesData_ = new UINT[indexCount];
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the vertex/index array");
	}
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

// get a name of the model's type
const std::string & ModelData::GetModelType() const
{
	return modelType_;
}


// Get the number of vertices
UINT ModelData::GetVertexCount(void) const
{
	return vertexCount_;
}

// Get the number of indices
UINT ModelData::GetIndexCount(void) const
{
	return indexCount_;
}



// returns a model world matrix
const DirectX::XMMATRIX & ModelData::GetWorldMatrix()
{
	DirectX::XMMATRIX beginPosition = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(scale_.x, scale_.y, scale_.z);
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(radianAngle_.y, radianAngle_.x, 0.0f);
	DirectX::XMMATRIX translate = DirectX::XMMatrixTranslation(position_.x, position_.y, position_.z);

	modelWorldMatrix_ = beginPosition * rotation * scale * translate;

	return modelWorldMatrix_;
}


// returns an identifier of the model
const std::string & ModelData::GetID() const
{
	return modelID_;
}


const DirectX::XMFLOAT3 & ModelData::GetPosition() const { return position_; }
const DirectX::XMFLOAT3 & ModelData::GetScale() const { return scale_; }
const DirectX::XMFLOAT2 & ModelData::GetRotation() const { return radianAngle_; }
const DirectX::XMFLOAT4 & ModelData::GetColor() const { return color_; };


// get a pointer to the model's vertices data array
VERTEX* ModelData::GetVerticesData()
{
	return pVerticesData_;
}

// get a pointer to the model's indices data array
UINT* ModelData::GetIndicesData()
{
	return pIndicesData_;
}

VERTEX** ModelData::GetAddressOfVerticesData()
{
	return &pVerticesData_;
}

UINT** ModelData::GetAddressOfIndicesData()
{
	return &pIndicesData_;
}



/////////////////////////////////////////////////////////////////////////////////////////
// 
//                           PUBLIC FUNCTIONS: SETTERS
//
/////////////////////////////////////////////////////////////////////////////////////////


void ModelData::SetVerticesData(const VERTEX* pModelData, UINT verticesCount)
{
	assert(verticesCount > 0);

	this->pVerticesData_ = new VERTEX[verticesCount];
	COM_ERROR_IF_FALSE(this->pVerticesData_, "can't allocate memory for the model vertex data");

	// copy each vertex data into the current model vertices array
	for (size_t i = 0; i < verticesCount; i++)
	{
		this->pVerticesData_[i] = pModelData[i];
	}
	
}

void ModelData::SetIndexData(const UINT* pIndicesData, UINT indicesCount)
{
	assert(indicesCount > 0);

	this->pIndicesData_ = new UINT[indicesCount];
	COM_ERROR_IF_FALSE(this->pIndicesData_, "can't allocate memory for the model index data");

	// copy each index data into the current model indices array
	for (size_t i = 0; i < indicesCount; i++)
	{
		this->pIndicesData_[i] = pIndicesData[i];
	}
}

void ModelData::SetVertexCount(UINT vertexCount)
{
	this->vertexCount_ = vertexCount;
}

void ModelData::SetIndexCount(UINT indexCount)
{
	this->indexCount_ = indexCount;
	SetVertexCount(this->indexCount_);
}

// set what kind of model this object is
void ModelData::SetModelType(const std::string& modelType)
{
	this->modelType_ = modelType;
	Log::Print(THIS_FUNC, this->modelType_.c_str());
}


// set an identifier of the model
void ModelData::SetID(const std::string& modelId)
{
	modelID_ = modelId;
}

// set model's position in the world
void ModelData::SetPosition(float x, float y, float z)
{
	position_.x = x;
	position_.y = y;
	position_.z = z;

	return;
}

// set model's scaling
void ModelData::SetScale(float x, float y, float z)
{
	scale_.x = x;
	scale_.y = y;
	scale_.z = z;

	return;
}

// set model's rotation
void ModelData::SetRotation(float radiansX, float radiansY)
{
	radianAngle_.x = radiansX;
	radianAngle_.y = radiansY;

	return;
}

// set model's color
void ModelData::SetColor(float red, float green, float blue, float alpha)
{
	color_.x = red;
	color_.y = green;
	color_.z = blue;
	color_.w = alpha;
}

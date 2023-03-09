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
	_DELETE(pModelData_);         // release the model vertices data
	_DELETE(pIndicesData_);       // release the model indices data
}



// memory allocation (we need it because we use DirectX::XM-objects)
void* ModelData::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);
	if (!ptr)
	{
		Log::Error(THIS_FUNC, "can't allocate the memory for object");
		return nullptr;
	}

	return ptr;
}

void ModelData::operator delete(void* p)
{
	_aligned_free(p);
}


// copy model's data from the original
void ModelData::operator= (ModelData* data)
{
	Log::Print(THIS_FUNC_EMPTY);
	this->SetID(data->GetID());  // initialize an identifier of the model


	this->vertexCount_ = data->GetVertexCount();
	this->indexCount_ = data->GetIndexCount();
	this->pModelData_ = data->GetModelData();
	this->pIndicesData_ = data->GetIndicesData();
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
VERTEX* ModelData::GetModelData()
{
	return pModelData_;
}

// get a pointer to the model's indices data array
UINT* ModelData::GetIndicesData()
{
	return pIndicesData_;
}

VERTEX** ModelData::GetAddressOfModelData()
{
	return &pModelData_;
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


void ModelData::SetModelData(VERTEX* pModelData)
{
	this->pModelData_ = pModelData;
}

void ModelData::SetIndexData(UINT* pIndicesData)
{
	this->pIndicesData_ = pIndicesData;
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

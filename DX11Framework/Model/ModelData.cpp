////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     ModelData.cpp
// 
// Created:      28.02.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "ModelData.h"


ModelData::ModelData()
	: modelWorldMatrix_(DirectX::XMMatrixIdentity()), // by default we set the model at the beginning of the world
	  position_(0, 0, 0),                    
	  scale_(1, 1, 1),
	  radianAngle_(0, 0, 0),
	  color_(1, 0, 1, 1)
{
	// compute beginning matrices for the model
	rotationMatrix_ = DirectX::XMMatrixRotationRollPitchYaw(radianAngle_.z, radianAngle_.y, radianAngle_.x);
	scalingMatrix_ = DirectX::XMMatrixScaling(scale_.x, scale_.y, scale_.z);
	translationMatrix_ = DirectX::XMMatrixTranslation(position_.x, position_.y, position_.z);

	// compute the final world matrix for the model
	ComputeWorldMatrix();

}

ModelData::ModelData(const ModelData & data)
{
}

ModelData::~ModelData()
{
	this->Shutdown();
}






////////////////////////////////////////////////////////////////////////////////////////////
// 
//                                PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////



// by the input value of vertices and indices allocates memory for a vertex and index array
// and setup the number of vertices and indices of this model
void ModelData::AllocateVerticesAndIndicesArrays(UINT vertexCount, UINT indexCount)
{
	try
	{
		// allocate memory for the arrays
		pVerticesData_ = new VERTEX[vertexCount];
		pIndicesData_ = new UINT[indexCount];

		// initialize vertex array to zeros at first
		memset(pVerticesData_, 0, (sizeof(VERTEX) * vertexCount));

		// load the index array with data
		for (UINT i = 0; i < indexCount; i++)
		{
			pIndicesData_[i] = i;
		}
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

} // end AllocateVerticesAndIndicesArrays

///////////////////////////////////////////////////////////

void ModelData::Shutdown()
{
	// release memory from the model's vertices/indices data
	_DELETE_ARR(pVerticesData_);
	_DELETE_ARR(pIndicesData_);

	return;
}

///////////////////////////////////////////////////////////

void* ModelData::operator new(size_t i)
{
	// memory allocation (we need it because we use DirectX::XM-objects)
	if (void* ptr = _aligned_malloc(i, 16))
	{
		return ptr;
	}
	
	Log::Error(THIS_FUNC, "can't allocate the memory for object");
	throw std::bad_alloc{};
}

///////////////////////////////////////////////////////////

void ModelData::operator delete(void* p) noexcept
{
	_aligned_free(p);
}



////////////////////////////////////////////////////////////////////////////////////////////
// 
//                           PUBLIC FUNCTIONS: GETTERS
//
////////////////////////////////////////////////////////////////////////////////////////////


const DirectX::XMMATRIX & ModelData::GetWorldMatrix()
{
	// returns a model's world matrix
	return modelWorldMatrix_;
	
}

///////////////////////////////////////////////////////////

const std::string & ModelData::GetPathToDataFile() const 
{
	// returns a path to data file of this model's type
	return pathToDataFile_;
}

///////////////////////////////////////////////////////////

const std::string & ModelData::GetID() const
{
	// returns an identifier of the model
	return modelID_;
}

///////////////////////////////////////////////////////////


//
// GETTERS for model's vertices/indices data
//

VERTEX* ModelData::GetVerticesData()
{
	// get a pointer to the model's vertices data array
	assert(pVerticesData_ != nullptr);
	return pVerticesData_;
}

///////////////////////////////////////////////////////////
UINT* ModelData::GetIndicesData()
{
	// get a pointer to the model's indices data array
	assert(pIndicesData_ != nullptr);
	return pIndicesData_;
}

///////////////////////////////////////////////////////////

VERTEX** ModelData::GetAddressOfVerticesData()
{
	return &pVerticesData_;
}

///////////////////////////////////////////////////////////

UINT** ModelData::GetAddressOfIndicesData()
{
	return &pIndicesData_;
}

///////////////////////////////////////////////////////////

UINT ModelData::GetVertexCount() const
{
	// Get the number of vertices
	assert(vertexCount_ != 0);
	return vertexCount_;
}

///////////////////////////////////////////////////////////

UINT ModelData::GetIndexCount() const
{
	// Get the number of indices
	assert(indexCount_ != 0);
	return indexCount_;
}

///////////////////////////////////////////////////////////


//
// GETTERS for model's position (in world)/scale/rotation/color/etc.
//
const DirectX::XMFLOAT3 & ModelData::GetPosition() const { return position_; }
const DirectX::XMFLOAT3 & ModelData::GetScale()    const { return scale_; }
const DirectX::XMFLOAT3 & ModelData::GetRotation() const { return radianAngle_; }
const DirectX::XMFLOAT4 & ModelData::GetColor()    const { return color_; };









////////////////////////////////////////////////////////////////////////////////////////////
// 
//                           PUBLIC FUNCTIONS: SETTERS
//
////////////////////////////////////////////////////////////////////////////////////////////


void ModelData::CopyVerticesData(const VERTEX* pVertexData, UINT verticesCount)
{
	assert(pVertexData != nullptr);
	assert(verticesCount > 0);

	try
	{
		// before allocation of new memory we have to release the used memory (if we have it)
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
	assert(indicesCount > 0);

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



void ModelData::SetPathToDataFile(const std::string & pathToDataFile)
{
	// setup a path to data file of this model's type (for example: /default/cube.txt)
	//
	// NOTE: some models don't necessarily have a path to data file (for example: Triangle)
	//       because its data is creating inside its class

	this->pathToDataFile_ = pathToDataFile;
}

// set an identifier of the model
void ModelData::SetID(const std::string& modelId)
{
	assert(modelId.empty() != true);
	modelID_ = modelId;
}


void ModelData::SetVertexCount(const UINT vertexCount)
{
	assert(vertexCount > 0);
	this->vertexCount_ = vertexCount;
}


void ModelData::SetIndexCount(const UINT indexCount)
{
	assert(indexCount > 0);
	this->indexCount_ = indexCount;
}

// set model's position in the world
void ModelData::SetPosition(const float x, const float y, const float z)
{
	position_.x = x;
	position_.y = y;
	position_.z = z;

	// compute a translation matrix for new translation values
	translationMatrix_ = DirectX::XMMatrixTranslation(position_.x, position_.y, position_.z);

	// compute the final world matrix for the model
	ComputeWorldMatrix();

	return;
}

// sets model's position in the world using XMFLOAT3 type
void ModelData::SetPosition(const DirectX::XMFLOAT3 & position)
{
	position_ = position;

	// compute a translation matrix for new translation values
	translationMatrix_ = DirectX::XMMatrixTranslation(position_.x, position_.y, position_.z);

	// compute the final world matrix for the model
	ComputeWorldMatrix();

	return;
}

// sets model's position in the world using XMFLOAT4 type
void ModelData::SetPosition(const DirectX::XMFLOAT4 & position)
{
	position_.x = position.x;
	position_.y = position.y;
	position_.z = position.z;

	// compute a translation matrix for new translation values
	translationMatrix_ = DirectX::XMMatrixTranslation(position_.x, position_.y, position_.z);

	// compute the final world matrix for the model
	ComputeWorldMatrix();

	return;
}

// this functions adjusts the current position of a model
void ModelData::AdjustPosition(const DirectX::XMFLOAT3 & translatePos)
{
	position_.x += translatePos.x;
	position_.y += translatePos.y;
	position_.z += translatePos.z;

	// compute a translation matrix for new translation values
	translationMatrix_ = DirectX::XMMatrixTranslation(position_.x, position_.y, position_.z);
	
	// compute the final world matrix for the model
	ComputeWorldMatrix();

	return;
}

// set model's scaling
void ModelData::SetScale(const float x, const float y, const float z)
{
	scale_.x = x;
	scale_.y = y;
	scale_.z = z;

	// compute a scaling matrix for new scaling values
	scalingMatrix_ = DirectX::XMMatrixScaling(scale_.x, scale_.y, scale_.z);

	// compute the final world matrix for the model
	ComputeWorldMatrix();

	return;
}

// set model's rotation (takes angles in radians as input)
// ATTENTION: rotation is performed around the corresponding axis;
void ModelData::SetRotation(const float radiansX, const float radiansY, const float radiansZ)
{
	radianAngle_.x = radiansX;
	radianAngle_.y = radiansY;
	radianAngle_.z = radiansZ;

	// compute a rotation matrix for new rotation values
	rotationMatrix_ = DirectX::XMMatrixRotationRollPitchYaw(radianAngle_.z, radianAngle_.y, radianAngle_.x);
	
	// compute the final world matrix for the model
	ComputeWorldMatrix();

	return;
}

// set model's rotation (takes angles in degrees as input);
// ATTENTION: rotation is performed around the corresponding axis
void ModelData::SetRotationInDegrees(const float angleX, const float angleY, const float angleZ)
{
	radianAngle_.x = DirectX::XMConvertToRadians(angleX);
	radianAngle_.y = DirectX::XMConvertToRadians(angleY);
	radianAngle_.z = DirectX::XMConvertToRadians(angleZ);

	// compute a rotation matrix for new rotation values
	rotationMatrix_ = DirectX::XMMatrixRotationRollPitchYaw(radianAngle_.z, radianAngle_.y, radianAngle_.x);

	// compute the final world matrix for the model
	ComputeWorldMatrix();

	return;
}

// set model's color
void ModelData::SetColor(float red, float green, float blue, float alpha)
{
	color_.x = red;
	color_.y = green;
	color_.z = blue;
	color_.w = alpha;

	return;
}

void ModelData::SetColor(const DirectX::XMFLOAT4 & color)
{
	color_ = color;
}

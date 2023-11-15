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




void ModelData::AllocateVerticesAndIndicesArrays(UINT vertexCount, UINT indexCount)
{
	// this function by the input value of vertices and indices allocates
	// memory for a vertex and index array
	// and setups the number of vertices and indices of this model
	//
	// this function is useful when we generate vertices/indices dynamically but
	// not from the data file

	try
	{
		// allocate memory for the arrays
		verticesCoordsArr_.resize(vertexCount);
		indicesArr_.resize(indexCount);

		// initialize vertex array to zeros at first
		memset(verticesCoordsArr_.data(), 0, (sizeof(VERTEX) * vertexCount));

		// load the index array with data
		for (UINT i = 0; i < indexCount; i++)
		{
			indicesArr_[i] = i;
		}

		// setup the number of vertices and indices
		this->SetVertexCount(vertexCount);
		this->SetIndexCount(indexCount);
	}
	catch (std::bad_alloc & e)
	{
		verticesCoordsArr_.clear();
		indicesArr_.clear();

		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the vertex/index array");
	}

	return;

} // end AllocateVerticesAndIndicesArrays

///////////////////////////////////////////////////////////

void ModelData::Shutdown()
{
	// release memory from the model's vertices/indices data
	verticesCoordsArr_.clear();
	indicesArr_.clear();

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

/*

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

*/

std::vector<VERTEX> & ModelData::GetVertices()
{
	return verticesCoordsArr_;
}

///////////////////////////////////////////////////////////

std::vector<UINT> & ModelData::GetIndices()
{
	return indicesArr_;
}

///////////////////////////////////////////////////////////

UINT ModelData::GetVertexCount() const
{
	// Get the number of vertices
	return vertexCount_;
}

///////////////////////////////////////////////////////////

UINT ModelData::GetIndexCount() const
{
	// Get the number of indices
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

	// remove old vertices data
	this->verticesCoordsArr_.clear();

	// allocate memory for new vertices
	this->verticesCoordsArr_.reserve(verticesCount);

	// copy vertices data
	memcpy(verticesCoordsArr_.data(), pVertexData, sizeof(VERTEX) * verticesCount);

	// setup the number of vertices in the model
	vertexCount_ = static_cast<UINT>(verticesCoordsArr_.size());
	
	return;
}

///////////////////////////////////////////////////////////

void ModelData::CopyIndicesData(const UINT* pIndicesData, UINT indicesCount)
{
	assert(pIndicesData != nullptr);
	assert(indicesCount > 0);

	// remove old indices data
	this->indicesArr_.clear();

	// allocate memory for new indices
	this->indicesArr_.reserve(indicesCount);

	// copy indices data
	memcpy(indicesArr_.data(), pIndicesData, sizeof(VERTEX) * indicesCount);

	// setup the number of indices in the model
	indexCount_ = static_cast<UINT>(indicesArr_.size());

	return;
}

///////////////////////////////////////////////////////////

void ModelData::SetPathToDataFile(const std::string & pathToDataFile)
{
	// setup a path to data file of this model's type (for example: /default/cube.txt)
	//
	// NOTE: some models don't necessarily have a path to data file (for example: Triangle)
	//       because its data is creating inside its class

	assert(pathToDataFile.empty() != true);
	this->pathToDataFile_ = pathToDataFile;
}

///////////////////////////////////////////////////////////

void ModelData::SetID(const std::string & modelId)
{
	// set an identifier of the model
	COM_ERROR_IF_FALSE(modelId.empty() != true, "wrong ID");
	modelID_ = modelId;
}

///////////////////////////////////////////////////////////

void ModelData::SetVertexCount(const UINT vertexCount)
{
	// set the number of vertices in the model
	assert(vertexCount > 0);
	vertexCount_ = vertexCount;
}

///////////////////////////////////////////////////////////

void ModelData::SetIndexCount(const UINT indexCount)
{
	// set the number of indices in the model
	assert(indexCount > 0);
	indexCount_ = indexCount;
}









////////////////////////////////////////////////////////////////////////////////////////////
//                       MODIFICATORS FOR A MODEL IN THE WORLD
////////////////////////////////////////////////////////////////////////////////////////////



void ModelData::SetPosition(const float x, const float y, const float z)
{
	// set model's position in the world
	position_.x = x;
	position_.y = y;
	position_.z = z;

	// compute a translation matrix for new translation values
	translationMatrix_ = DirectX::XMMatrixTranslation(position_.x, position_.y, position_.z);

	// compute the final world matrix for the model
	ComputeWorldMatrix();

	return;
}

///////////////////////////////////////////////////////////

void ModelData::SetPosition(const DirectX::XMFLOAT3 & position)
{
	// sets model's position in the world using XMFLOAT3 type

	position_ = position;

	// compute a translation matrix for new translation values
	translationMatrix_ = DirectX::XMMatrixTranslation(position_.x, position_.y, position_.z);

	// compute the final world matrix for the model
	ComputeWorldMatrix();

	return;
}

///////////////////////////////////////////////////////////

void ModelData::SetPosition(const DirectX::XMFLOAT4 & position)
{
	// sets model's position in the world using XMFLOAT4 type

	position_.x = position.x;
	position_.y = position.y;
	position_.z = position.z;

	// compute a translation matrix for new translation values
	translationMatrix_ = DirectX::XMMatrixTranslation(position_.x, position_.y, position_.z);

	// compute the final world matrix for the model
	ComputeWorldMatrix();

	return;
}

///////////////////////////////////////////////////////////

void ModelData::AdjustPosition(const DirectX::XMFLOAT3 & translatePos)
{
	// this functions adjusts the current position of a model

	position_.x += translatePos.x;
	position_.y += translatePos.y;
	position_.z += translatePos.z;

	// compute a translation matrix for new translation values
	translationMatrix_ = DirectX::XMMatrixTranslation(position_.x, position_.y, position_.z);
	
	// compute the final world matrix for the model
	ComputeWorldMatrix();

	return;
}

///////////////////////////////////////////////////////////

void ModelData::SetScale(const float x, const float y, const float z)
{
	// set model's scaling

	scale_.x = x;
	scale_.y = y;
	scale_.z = z;

	// compute a scaling matrix for new scaling values
	scalingMatrix_ = DirectX::XMMatrixScaling(scale_.x, scale_.y, scale_.z);

	// compute the final world matrix for the model
	ComputeWorldMatrix();

	return;
}

///////////////////////////////////////////////////////////

void ModelData::SetRotation(const float radiansX, const float radiansY, const float radiansZ)
{
	// set model's rotation (takes angles in radians as input)
	// ATTENTION: rotation is performed around the corresponding axis;
	radianAngle_.x = radiansX;
	radianAngle_.y = radiansY;
	radianAngle_.z = radiansZ;

	// compute a rotation matrix for new rotation values
	rotationMatrix_ = DirectX::XMMatrixRotationRollPitchYaw(radianAngle_.z, radianAngle_.y, radianAngle_.x);
	
	// compute the final world matrix for the model
	ComputeWorldMatrix();

	return;
}

///////////////////////////////////////////////////////////

void ModelData::SetRotationInDegrees(const float angleX, const float angleY, const float angleZ)
{
	// set model's rotation (takes angles in degrees as input);
	// ATTENTION: rotation is performed around the corresponding axis

	radianAngle_.x = DirectX::XMConvertToRadians(angleX);
	radianAngle_.y = DirectX::XMConvertToRadians(angleY);
	radianAngle_.z = DirectX::XMConvertToRadians(angleZ);

	// compute a rotation matrix for new rotation values
	rotationMatrix_ = DirectX::XMMatrixRotationRollPitchYaw(radianAngle_.z, radianAngle_.y, radianAngle_.x);

	// compute the final world matrix for the model
	ComputeWorldMatrix();

	return;
}

///////////////////////////////////////////////////////////

void ModelData::SetColor(float red, float green, float blue, float alpha)
{
	// set model's color

	color_.x = red;
	color_.y = green;
	color_.z = blue;
	color_.w = alpha;

	return;
}

///////////////////////////////////////////////////////////

void ModelData::SetColor(const DirectX::XMFLOAT4 & color)
{
	// set model's color
	color_ = color;
}

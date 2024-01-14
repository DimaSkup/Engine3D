////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     GameObjectData.h
// Description:  implementation of functional for GameObjectData class;
//
// Created:      28.02.23
////////////////////////////////////////////////////////////////////////////////////////////

#include "GameObjectData.h"



GameObjectData::GameObjectData()
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

GameObjectData::GameObjectData(const GameObjectData & data)
{
}

GameObjectData::~GameObjectData()
{
}

///////////////////////////////////////////////////////////

void* GameObjectData::operator new(size_t i)
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

void GameObjectData::operator delete(void* p)
{
	_aligned_free(p);
}





////////////////////////////////////////////////////////////////////////////////////////////
// 
//                                 PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////



const DirectX::XMMATRIX & GameObjectData::GetWorldMatrix()
{
	// returns a model's world matrix
	return modelWorldMatrix_;

}

///////////////////////////////////////////////////////////

//
// GETTERS for model's position (in world)/scale/rotation/color/etc.
//
const DirectX::XMFLOAT3 & GameObjectData::GetPosition() const { return position_; }
const DirectX::XMFLOAT3 & GameObjectData::GetScale()    const { return scale_; }
const DirectX::XMFLOAT3 & GameObjectData::GetRotation() const { return radianAngle_; }
const DirectX::XMFLOAT4 & GameObjectData::GetColor()    const { return color_; };






////////////////////////////////////////////////////////////////////////////////////////////
//
//                MODIFICATORS (SETTERS) FOR A GAME OBJECT IN THE WORLD
//
////////////////////////////////////////////////////////////////////////////////////////////

void GameObjectData::SetWorldMatrix(const DirectX::XMMATRIX & newWorldMatrix)
{
	// set a new world matrix for the game object
	this->modelWorldMatrix_ = newWorldMatrix;
}

void GameObjectData::SetPosition(const float x, const float y, const float z)
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

void GameObjectData::SetPosition(const DirectX::XMFLOAT3 & position)
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

void GameObjectData::SetPosition(const DirectX::XMFLOAT4 & position)
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

void GameObjectData::AdjustPosition(const DirectX::XMFLOAT3 & translatePos)
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

void GameObjectData::SetScale(const float x, const float y, const float z)
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

void GameObjectData::SetRotationInRad(const float radiansX, const float radiansY, const float radiansZ)
{
	// set model's rotation (takes angles in radians as input)
	// ATTENTION: rotation is performed around the corresponding axis;
	radianAngle_.x = radiansX;
	radianAngle_.y = radiansY;
	radianAngle_.z = radiansZ;

	// compute a rotation matrix for new rotation values
	rotationMatrix_ = DirectX::XMMatrixRotationRollPitchYaw(radianAngle_.x, radianAngle_.y, radianAngle_.z);

	// compute the final world matrix for the model
	ComputeWorldMatrix();

	return;
}

///////////////////////////////////////////////////////////

void GameObjectData::SetRotationInDeg(const float angleX, const float angleY, const float angleZ)
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

void GameObjectData::AdjustRotationInRad(const float ax, const float ay, const float az)
{
	// this function adjusts model's rotation (takes angles in radians as input)
	// ATTENTION: rotation is performed around the corresponding axis;
	radianAngle_.x += ax;
	radianAngle_.y += ay;
	radianAngle_.z += az;

	// compute a rotation matrix for new rotation values
	rotationMatrix_ = DirectX::XMMatrixRotationRollPitchYaw(radianAngle_.z, radianAngle_.y, radianAngle_.x);

	// compute the final world matrix for the model
	ComputeWorldMatrix();
}

///////////////////////////////////////////////////////////

void GameObjectData::AdjustRotationInRad(const DirectX::XMFLOAT3 & angles)
{
	// this function adjusts model's rotation (takes angles in radians as input)
	// ATTENTION: rotation is performed around the corresponding axis;
	radianAngle_.x += angles.x;
	radianAngle_.y += angles.y;
	radianAngle_.z += angles.z;

	// compute a rotation matrix for new rotation values
	rotationMatrix_ = DirectX::XMMatrixRotationRollPitchYaw(radianAngle_.z, radianAngle_.y, radianAngle_.x);

	// compute the final world matrix for the model
	ComputeWorldMatrix();
}

///////////////////////////////////////////////////////////

void GameObjectData::AdjustRotationInDeg(const float ax, const float ay, const float az)
{
	// this function adjusts model's rotation (takes angles in degrees as input);
	// ATTENTION: rotation is performed around the corresponding axis

	radianAngle_.x += DirectX::XMConvertToRadians(ax);
	radianAngle_.y += DirectX::XMConvertToRadians(ay);
	radianAngle_.z += DirectX::XMConvertToRadians(az);

	// compute a rotation matrix for new rotation values
	rotationMatrix_ = DirectX::XMMatrixRotationRollPitchYaw(radianAngle_.z, radianAngle_.y, radianAngle_.x);

	// compute the final world matrix for the model
	ComputeWorldMatrix();
}

///////////////////////////////////////////////////////////

void GameObjectData::AdjustRotationInDeg(const DirectX::XMFLOAT3 & angles)
{
	// this function adjusts model's rotation (takes angles in degrees as input);
	// ATTENTION: rotation is performed around the corresponding axis

	radianAngle_.x += DirectX::XMConvertToRadians(angles.x);
	radianAngle_.y += DirectX::XMConvertToRadians(angles.y);
	radianAngle_.z += DirectX::XMConvertToRadians(angles.z);

	// compute a rotation matrix for new rotation values
	rotationMatrix_ = DirectX::XMMatrixRotationRollPitchYaw(radianAngle_.z, radianAngle_.y, radianAngle_.x);

	// compute the final world matrix for the model
	ComputeWorldMatrix();
}

///////////////////////////////////////////////////////////

void GameObjectData::SetColor(float red, float green, float blue, float alpha)
{
	// set model's color

	color_.x = red;
	color_.y = green;
	color_.z = blue;
	color_.w = alpha;

	return;
}

///////////////////////////////////////////////////////////

void GameObjectData::SetColor(const DirectX::XMFLOAT4 & color)
{
	// set model's color
	color_ = color;
}

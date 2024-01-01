////////////////////////////////////////////////////////////////////
// Filename:     GameObjectData.h
// Description:  this class is a DTO for the game object data
//               (but has a bit computations for the world matrix)
//
// Created:      28.02.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////


#include <DirectXMath.h>
#include <d3d11.h>

#include "../Engine/log.h"


//////////////////////////////////
// Class name: GameObjectData
//////////////////////////////////
class GameObjectData
{
public:
	GameObjectData();
	GameObjectData(const GameObjectData & data);
	~GameObjectData();

	// memory allocation (we need it because we use DirectX::XM-objects)
	void* operator new(std::size_t count);     // a replaceable allocation function
	void operator delete(void* p);


	//-----------------------------------------
	//               GETTERS
	//-----------------------------------------
	const DirectX::XMMATRIX & GetWorldMatrix();

	// get model's position/scale/rotation/color/etc.
	const DirectX::XMFLOAT3 & GetPosition() const;
	const DirectX::XMFLOAT3 & GetScale()    const;
	const DirectX::XMFLOAT3 & GetRotation() const;
	const DirectX::XMFLOAT4 & GetColor()    const;



	//-----------------------------------------
	//               SETTERS
	//-----------------------------------------
	void SetWorldMatrix(const DirectX::XMMATRIX & newWorldMatrix);
	void SetPosition(const float x, const float y, const float z);
	void SetPosition(const DirectX::XMFLOAT3 & position);
	void SetPosition(const DirectX::XMFLOAT4 & position);
	void AdjustPosition(const DirectX::XMFLOAT3 & translatePos);

	void SetRotationInRad(const float radianX, const float radianY, const float radianZ);
	void AdjustRotationInRad(const float ax, const float ay, const float az);
	void AdjustRotationInRad(const DirectX::XMFLOAT3 & angles);
	void SetRotationInDeg(const float angleX, const float angleY, const float angleZ);
	void AdjustRotationInDeg(const float ax, const float ay, const float az);
	void AdjustRotationInDeg(const DirectX::XMFLOAT3 & angles);

	void SetScale(const float x, const float y, const float z);

	void SetColor(float red, float green, float blue, float alpha);
	void SetColor(const DirectX::XMFLOAT4 & color);


	// when we does some manipulations with model we have to compute a new world matrix for this model
	inline void ComputeWorldMatrix()
	{
		modelWorldMatrix_ = scalingMatrix_ * rotationMatrix_ * translationMatrix_;
	}


private:
	DirectX::XMMATRIX modelWorldMatrix_;
	DirectX::XMMATRIX rotationMatrix_;
	DirectX::XMMATRIX scalingMatrix_;
	DirectX::XMMATRIX translationMatrix_;

	// model properties in the world
	DirectX::XMFLOAT3 position_;        // position of the model in the world
	DirectX::XMFLOAT3 scale_;           // scale of the model
	DirectX::XMFLOAT3 radianAngle_;     // current angles of the model rotation (in radians)
	DirectX::XMFLOAT4 color_;           // color of the model
};
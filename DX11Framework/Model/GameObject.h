#pragma once

#include <memory>

#include "Model.h"

class GameObject
{
public:
	GameObject();
	GameObject(const GameObject & origin);

	bool Initialize(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext);

	void Shutdown();


	/////////////////////////////  SETTERS/GETTERS  /////////////////////////////

	// set/return a game object's ID
	const std::string & GetID() const;
	void SetID(const std::string& newID);


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
	void SetRotationInDeg(const float angleX, const float angleY, const float angleZ);

	void AdjustRotationInRad(const float ax, const float ay, const float az);
	void AdjustRotationInRad(const DirectX::XMFLOAT3 & angles);
	void AdjustRotationInDeg(const float ax, const float ay, const float az);
	void AdjustRotationInDeg(const DirectX::XMFLOAT3 & angles);

	void SetScale(const float x, const float y, const float z);

	void SetColor(float red, float green, float blue, float alpha);
	void SetColor(const DirectX::XMFLOAT4 & color);



protected:
	virtual void UpdateMatrix();
	void UpdateDirectionsVectors();

protected:
	std::string ID_{ "no_ID" };         // an identifier of the game object

	DirectX::XMMATRIX worldMatrix_;
	DirectX::XMMATRIX rotationMatrix_;
	DirectX::XMMATRIX scalingMatrix_;
	DirectX::XMMATRIX translationMatrix_;

	// model properties in the world
	DirectX::XMFLOAT3 position_;        // position of the model in the world
	DirectX::XMFLOAT3 scale_;           // scale of the model
	DirectX::XMFLOAT3 rotation_;        // current angles of the model rotation (in radians)
	DirectX::XMFLOAT4 color_;           // color of the model

	DirectX::XMVECTOR vecForward_;      // the current forward direction of the game obj
	DirectX::XMVECTOR vecLeft_;
	DirectX::XMVECTOR vecRight_;
	DirectX::XMVECTOR vecBackward_;

	const DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR_  = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_UP_VECTOR_       = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_BACKWARD_VECTOR_ = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_LEFT_VECTOR_     = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_RIGHT_VECTOR_    = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
};
#pragma once

#include <memory>

class GameObject
{
public:
	// according to these flags we define what type of game object we want to create
	// so we handle them in a slighty different way
	enum GameObjectType
	{
		GAME_OBJ_UNKNOWN,        // default
		GAME_OBJ_ZONE_ELEMENT,   // terrain, sky dome / sky box, clouds, etc.
		GAME_OBJ_RENDERABLE,     // 3D models: cubes, spheres, triangles, custom models (imported), etc.
		GAME_OBJ_SPRITE,         // simple 2D sprites
		GAME_OBJ_CAMERA,         // editor camera/game camera/etc.
		GAME_OBJ_LIGHT_SRC,      // light source: diffuse, point, spotlight, etc.
	};

public:
	GameObject();
	GameObject(const GameObject & origin);
	virtual ~GameObject();

	// memory allocation (we need it because we use DirectX::XM-objects)
	void* operator new(std::size_t count);     // a replaceable allocation function
	void operator delete(void* p);



	//-----------------------------------------
	//               GETTERS
	//-----------------------------------------
	const std::string & GetID() const;
	const GameObjectType GetType() const;

	const DirectX::XMMATRIX & GetWorldMatrix();

	// get model's position/scale/rotation/color/etc.
	const DirectX::XMFLOAT3 & GetPosition() const;
	const DirectX::XMFLOAT3 & GetScale()    const;
	const DirectX::XMFLOAT3 & GetRotation() const;
	const DirectX::XMFLOAT3 & GetColor()    const;

	// get directions vectors
	const DirectX::XMVECTOR & GetForwardVector()  const;
	const DirectX::XMVECTOR & GetRightVector()    const;
	const DirectX::XMVECTOR & GetBackwardVector() const;
	const DirectX::XMVECTOR & GetLeftVector()     const;




	//-----------------------------------------
	//               SETTERS
	//-----------------------------------------
	void SetID(const std::string& newID);
	void SetType(const GameObjectType type);

	void SetWorldMatrix(const DirectX::XMMATRIX & newWorldMatrix);
	void SetPosition(const float x, const float y, const float z);
	void SetPosition(const DirectX::XMFLOAT3 & position);
	void SetPosition(const DirectX::XMFLOAT4 & position);
	void AdjustPosition(const DirectX::XMFLOAT3 & translatePos);
	void AdjustPosition(const float x, const float y, const float z);
	void AdjustPosition(const DirectX::XMVECTOR & translationVector);

	void SetRotationInRad(const float radianX, const float radianY, const float radianZ);
	void SetRotationInDeg(const float angleX, const float angleY, const float angleZ);

	void AdjustRotationInRad(const float ax, const float ay, const float az);
	void AdjustRotationInRad(const DirectX::XMFLOAT3 & angles);
	void AdjustRotationInDeg(const float ax, const float ay, const float az);
	void AdjustRotationInDeg(const DirectX::XMFLOAT3 & angles);

	void SetScale(const float x, const float y, const float z);

	void SetColor(const float red, const float green, const float blue);
	void SetColor(const DirectX::XMFLOAT3 & color);





protected:
	virtual void UpdateMatrix();
	void UpdateDirectionsVectors();

protected:
	DirectX::XMMATRIX worldMatrix_;
	DirectX::XMMATRIX rotationMatrix_;
	DirectX::XMMATRIX scalingMatrix_;
	DirectX::XMMATRIX translationMatrix_;

	// model properties in the world
	DirectX::XMFLOAT3 position_;        // position of the model in the world
	DirectX::XMFLOAT3 scale_;           // scale of the model
	DirectX::XMFLOAT3 rotation_;        // current angles of the model rotation (in radians)
	DirectX::XMFLOAT3 color_;           // color of the model

	DirectX::XMVECTOR vecForward_;      // the current forward direction of the game obj
	DirectX::XMVECTOR vecLeft_;
	DirectX::XMVECTOR vecRight_;
	DirectX::XMVECTOR vecBackward_;

	const DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR_  = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_UP_VECTOR_       = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_BACKWARD_VECTOR_ = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_LEFT_VECTOR_     = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_RIGHT_VECTOR_    = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	std::string ID_{ "no_ID" };                      // an identifier of the game object
	GameObjectType gameObjType_ = GAME_OBJ_UNKNOWN;  // a type of this game object (it can be renderable game object, zone element, or sprite)
};
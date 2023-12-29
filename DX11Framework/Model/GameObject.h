#pragma once

#include <memory>

#include "Model.h"
#include "GameObjectData.h"

class GameObject
{
public:
	GameObject(Model* pModel);

	bool Initialize(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext);

	void Shutdown();

	void Render(D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void RenderSprite();


	/////////////////////////////  SETTERS/GETTERS  /////////////////////////////

	// set/return a game object's ID
	const std::string & GetID() const;
	void SetID(const std::string& newID);

	// relate some model to this game object
	void SetModel(Model* pModel);


	/////////////////////////////  INLINE GETTERS  /////////////////////////////

	inline Model* GetModel() const
	{
		// returns a pointer to the model of the game object
		COM_ERROR_IF_NULLPTR(this, "this == nullptr");
		return this->pModel_;
	}

	inline GameObjectData* GetData() const
	{
		// returns a pointer to an object which contains the model's data
		return pGameObjData_.get();
	}

	inline DataContainerForShaders* GetDataContainerForShaders() const
	{
		return pModel_->GetDataContainerForShaders();
	}

protected:
	Model* pModel_ = nullptr;
	std::unique_ptr<GameObjectData> pGameObjData_;

	std::string ID_{ "no_ID" };
};
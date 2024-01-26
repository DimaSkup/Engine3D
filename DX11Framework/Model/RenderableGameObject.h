#pragma once

#include "GameObject.h"

class RenderableGameObject : public GameObject
{
public:
	RenderableGameObject(Model* pModel);
	~RenderableGameObject();

	//bool Initialize(const std::string & filePath, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	void Render(D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void RenderSprite();

	virtual void UpdateMatrix() override;

	Model* GetModel() const;
	void SetModel(Model* pModel);  	// relate some model to this game object

private:
	Model* pModel_ = nullptr;
};
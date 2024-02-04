#pragma once

#include "GameObject.h"
#include <d3d11.h>
#include <string>

class RenderableGameObject : public GameObject
{
public:
	virtual ~RenderableGameObject();

	virtual bool Initialize(const std::string & filePath) = 0;
	virtual void Render(const D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) = 0;

	virtual void UpdateMatrix() override;
	virtual const std::string & GetModelType() = 0;

protected:
	DirectX::XMFLOAT3 color_;           // color of the model
};
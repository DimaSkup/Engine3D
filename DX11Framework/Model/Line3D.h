/////////////////////////////////////////////////////////////////////
// Filename:    Line3D.h
// Description: this class is a representation of a 3D line
// Revising:    20.10.23
/////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////

#include <vector>

#include "../Model/Model.h"


//////////////////////////////////
// Class name: Line3D
//////////////////////////////////
class Line3D : public Model
{
public:
	Line3D(ModelInitializerInterface* pModelInitializer);
	~Line3D();

	virtual bool Initialize(ID3D11Device* pDevice) override;

	virtual void Render(ID3D11DeviceContext* pDeviceContext,
		D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) override;

	// for configuring model before initialization
	void SetStartPoint(const float x, const float y, const float z);
	void SetEndPoint(const float x, const float y, const float z);

	void SetStartPoint(const DirectX::XMFLOAT3 & startPos);
	void SetEndPoint(const DirectX::XMFLOAT3 & endPos);



private:
	VERTEX startPoint_;
	VERTEX endPoint_;
};
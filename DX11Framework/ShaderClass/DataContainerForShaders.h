#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "../Render/lightclass.h"

struct DataContainerForShaders
{
	// model data
	UINT indexCount;                      // a number of indices of a model
	DirectX::XMMATRIX world;              
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX orthoOrProj;        // orthogonal or projection matrix
	DirectX::XMFLOAT4 modelColor;         // for ColorShaderClass; a color of the model
	ID3D11ShaderResourceView* const* ppTextures = nullptr;

	// camera data
	DirectX::XMFLOAT3 cameraPos;

	// light data
	LightClass* pDiffuseLightSources = nullptr;
	DirectX::XMFLOAT4* pPointLightsPositions = nullptr;
	DirectX::XMFLOAT4* pPointLightsColors = nullptr;

	// sky dome / sky box data
	DirectX::XMFLOAT4 skyDomeApexColor;
	DirectX::XMFLOAT4 skyDomeCenterColor;

	// sky plane data
	DirectX::XMFLOAT4 skyPlanesTranslation;  // first and second clouds transtaions by X-axis and Z-axis respectively (x,y -- first cloud; z,w -- second cloud)
	float skyPlanesBrightness = 0.0f;        
};

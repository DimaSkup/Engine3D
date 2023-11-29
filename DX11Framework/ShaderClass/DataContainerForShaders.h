#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

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
	std::vector<LightClass*>* ptrToDiffuseLightsArr = nullptr;
	std::vector<LightClass*>* ptrToPointLightsArr = nullptr;
	//LightClass* const* pDiffuseLightSources = nullptr;
	//LightClass* const* pPointLightSources = nullptr;
	//UINT numDiffuseLights = 0;                            // current number of the diffuse light sources on the scene (for example: sun)
	//UINT numPointLights = 0;                              // current number of the point light sources on the scene (for example: light bulb, candle)

	// sky dome / sky box data
	DirectX::XMFLOAT4 skyDomeApexColor;
	DirectX::XMFLOAT4 skyDomeCenterColor;

	// sky plane data
	DirectX::XMFLOAT4 skyPlanesTranslation;  // first and second clouds transtaions by X-axis and Z-axis respectively (x,y -- first cloud; z,w -- second cloud)
	float skyPlanesBrightness = 0.0f;        
};

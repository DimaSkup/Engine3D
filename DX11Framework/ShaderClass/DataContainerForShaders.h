///////////////////////////////////////////////////////////////////////////////////////////
// Filename:      DataContainerForShaders.h
// Description:   this struct is a common DTO (data transfer object) for all the shaders;
//                
//                For example:  each model and mesh sets its own data into this container
//                              from the one side and from the another each shader class
//                              gets this data and setup the rendering pipeline with it for
//                              rendering using HLSL shaders;
//
//                Note:         each ModelsToShaderMediator creates and initializes
//                              its own data_container_for_shaders inside its constructor;
//                              so if you are looking for the instance of DataContainerForShaders
//                              you can find it there;
//
///////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <map>

#include "../Render/lightclass.h"

struct DataContainerForShaders
{
	// model data
	UINT indexCount;                      // a number of indices of a model
	DirectX::XMMATRIX world;              
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX orthoOrProj;        // orthogonal or projection matrix
	DirectX::XMFLOAT4 modelColor;         // for ColorShaderClass; a color of the model
	std::map<std::string, ID3D11ShaderResourceView**> texturesMap;
	//std::vector<std::unique_ptr<TextureClass>> & texturesArr;
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

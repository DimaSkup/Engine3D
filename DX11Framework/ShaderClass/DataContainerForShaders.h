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
	DirectX::XMMATRIX WVP;                // (is used for 3D rendering): world * view * projection
	DirectX::XMMATRIX WVO;                // (is used for 2D rendering): this matrix is already transposed so it == transpose(main_world * baseView * ortho) 
	DirectX::XMMATRIX world_main_matrix;  // a matrix of all the world
	DirectX::XMMATRIX world;              // a world matrix of a particular model 
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX baseView;           // is used for 2D rendering (for sprites or GUI)
	DirectX::XMMATRIX projection;         // projection matrix
	DirectX::XMMATRIX ortho;              // orthogonal matrix
	DirectX::XMMATRIX viewProj;           // view_matrix * projection_matrix (or view * ortho)
	
	DirectX::XMMATRIX reflectionMatrix;   // a matrix which is used for rendering of pranar reflections
	DirectX::XMFLOAT3 color;              // just RGB color for different purposes
	std::map<std::string, ID3D11ShaderResourceView**> texturesMap;
	//std::vector<std::unique_ptr<TextureClass>> & texturesArr;
	ID3D11ShaderResourceView* const* ppTextures = nullptr;

	// camera data
	DirectX::XMFLOAT3 cameraPos;

	// light data
	std::vector<LightClass*>* ptrToDiffuseLightsArr = nullptr;
	std::vector<LightClass*>* ptrToPointLightsArr = nullptr;

	// sky dome / sky box data
	DirectX::XMFLOAT4 skyDomeApexColor;
	DirectX::XMFLOAT4 skyDomeCenterColor;

	// sky plane data
	DirectX::XMFLOAT4 skyPlanesTranslation;  // first and second clouds transtaions by X-axis and Z-axis respectively (x,y -- first cloud; z,w -- second cloud)
	float skyPlanesBrightness = 0.0f;    

	// params to control the fog
	DirectX::XMFLOAT3 fogColor{ 0.5f, 0.5f, 0.5f };  // grey
	bool  fogEnabled = true;
	float fogStart = 0.0f;
	float fogRange = 0.0f;
	float fogRange_inv = 0.0f;  // (1 / fogRange) we use this value for faster computation of the fog degree on pixel

	// parameter to control alpha clipping 
	bool useAlphaClip = false;   

	// DEBUG PARAMS
	bool debugNormals = false;
};

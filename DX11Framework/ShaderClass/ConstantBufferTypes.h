#pragma once

#include <DirectXMath.h>

struct ConstantMatrixBuffer_VS_WVP
{
	// the result of: world_matrix * view_matrix * projection_matrix 
	// OR           : main_world_matrix * base_view_matrix * ortho_matrix
	DirectX::XMMATRIX WVP;    
};

// a constant matrix buffer structure for vertex shaders
struct ConstantMatrixBuffer_VS
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};

struct ConstantColorBuffer_VS
{
	DirectX::XMFLOAT4 rgbaColor;
};



// a constant matrix buffer structure for the font vertex shader
struct ConstantMatrixBuffer_FontVS
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX ortho;
};


// a constant buffer which contains colours are used inside the font pixel shader
struct ConstantPixelBuffer_FontPS
{
	DirectX::XMFLOAT4 pixelColor;         // UI text colour
};


// a constant buffer structure that holds an alpha value for the texture pixel shader
struct ConstantAlphaBuffer_TexturePS
{
	float alpha = 1.0f;
};

// a constant light buffer structure for the terrain lighting
struct ConstantTerrainLightBuffer_TerrainPS
{
	DirectX::XMFLOAT4 ambientColor;       // a common light of the terrain
	DirectX::XMFLOAT4 diffuseColor;       // color of the main directed light
	DirectX::XMFLOAT3 lightDirection;     // a direction of the diffuse light
};

// a constant light buffer structure for the light pixel shader
// ATTENTION: must have the same structure both in the shader class and in the pixel shader (const buffer)
struct ConstantLightBuffer_LightPS
{
	DirectX::XMFLOAT4 ambientColor;       // a common light of the scene
	DirectX::XMFLOAT4 diffuseColor;       // color of the main directed light
	DirectX::XMFLOAT3 lightDirection;     // a direction of the diffuse light
	float             specularPower;      // the intensity of specular light
	DirectX::XMFLOAT4 specularColor;      // the color of specular light
};


// a light constant buffer structur for the bump map pixel shader
struct ConstantLightBuffer_BumpMapPS
{
	DirectX::XMFLOAT4 diffuseColor;
	DirectX::XMFLOAT3 lightDirection;
	float padding;
};

// a constant camera buffer structure for the light vertex shader
struct ConstantCameraBuffer_LightVS
{
	DirectX::XMFLOAT3 cameraPosition;
	float padding = 0.0f;                        // we need the padding because the size of this struct must be a multiple of 16
};


// a constant sky plane buffer contains the translation coordinates for 
// the clouds as well as the overall brightness of the clouds
struct ConstantSkyPlaneBuffer_PS
{
	float firstTranslationX = 0.0f; 
	float firstTranslationZ = 0.0f;
	float secondTranslationX = 0.0f;
	float secondTranslationZ = 0.0f;
	float brightness = 0.0f;
	float padding[3] = { 0.0f };
};
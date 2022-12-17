#pragma once

#include <DirectXMath.h>

// a constant matrix buffer structure for vertex shaders
struct ConstantMatrixBuffer_VS
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};


// a constant light buffer structure for the light pixel shader
struct ConstantLightBuffer_LightPS
{
	DirectX::XMFLOAT4 diffuseColor;    // a main directed light
	DirectX::XMFLOAT4 ambientColor;    // a common light of the scene
	DirectX::XMFLOAT3 lightDirection;  // a direction of the diffuse light
	float             specularPower;   // the intensity of specular light
	DirectX::XMFLOAT4 specularColor;   // the color of specular light
};

// a constant camere buffer structure for the light vertex shader
struct ConstantCameraBuffer_LightVS
{
	DirectX::XMFLOAT3 cameraPosition;
	float padding;                     // we need the padding because the size of this struct must be a multiple of 16
};
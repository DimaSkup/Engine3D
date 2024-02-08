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




// a constant buffer structure that holds an alpha value for the texture pixel shader
struct ConstantAlphaBuffer_TexturePS
{
	float alpha = 1.0f;
};

// a constant camera buffer structure for the light vertex shader
struct ConstantCameraBufferType
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
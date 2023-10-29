#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

struct DataContainerForShaders
{
	UINT indexCount;                      // a number of indices of a model
	DirectX::XMMATRIX world;              
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX orthoOrProj;        // orthogonal or projection matrix
	DirectX::XMFLOAT4 modelColor;         // for ColorShaderClass; a color of the model
	ID3D11ShaderResourceView* const* ppTextures;
};

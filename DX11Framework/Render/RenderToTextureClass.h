////////////////////////////////////////////////////////////////////////////////////////////
// Filename:       RenderToTextureClass.h
// 
// Description:    Render to texture allows you to render your scene to a texture resource
//                 instead of just the back buffer. It also allows you to retrieve 
//                 the data rendered to it in the form of a regular texture
//
// Created:        20.09.23
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <DirectXMath.h>

#include "../Engine/Log.h"

using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////////////////
// Class name: RenderToTextureClass
////////////////////////////////////////////////////////////////////////////////////////////
class RenderToTextureClass
{
public:
	RenderToTextureClass();
	~RenderToTextureClass();

	bool Initialize(ID3D11Device* pDevice,
		const UINT textureWidth,
		const UINT textureHeight,
		const float screenDepth,
		const float screenNear,
		const int format);

	void SetRenderTarget(ID3D11DeviceContext* pDeviceContext);

	void ClearRenderTarget(ID3D11DeviceContext*, const DirectX::XMFLOAT4 & rgbaColor);

	ID3D11ShaderResourceView* const* GetShaderResourceView() const;

	void GetProjectionMatrix(DirectX::XMMATRIX & proj);
	void GetOrthoMatrix(DirectX::XMMATRIX & ortho);

	UINT GetTextureWidth() const;
	UINT GetTextureHeight() const;

private:  // restrict a copying of this class instance 
	RenderToTextureClass(const RenderToTextureClass & obj);
	RenderToTextureClass & operator=(const RenderToTextureClass & obj);

private:
	UINT textureWidth_ = 0;
	UINT textureHeight_ = 0;
	ID3D11Texture2D* pRenderTargetTexture_ = nullptr;
	ID3D11RenderTargetView* pRenderTargetView_ = nullptr;
	ID3D11ShaderResourceView* pShaderResourceView_ = nullptr;
	ID3D11Texture2D* pDepthStencilBuffer_ = nullptr;
	ID3D11DepthStencilView* pDepthStencilView_ = nullptr;
	D3D11_VIEWPORT viewport_{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	DirectX::XMMATRIX projectionMatrix_ = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX orthoMatrix_ = DirectX::XMMatrixIdentity();

}; // end RenderToTextureClass

////////////////////////////////////////////////////////////////////////////////////////////
// Filename:       RenderToTextureClass.cpp
// 
// Description:    Render to texture allows you to render your scene to a texture resource
//                 instead of just the back buffer. It also allows you to retrieve 
//                 the data rendered to it in the form of a regular texture
//
// Created:        20.09.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "RenderToTextureClass.h"

RenderToTextureClass::RenderToTextureClass()
{
}

RenderToTextureClass::~RenderToTextureClass()
{
	_RELEASE(pDepthStencilView_);
	_RELEASE(pDepthStencilBuffer_);
	_RELEASE(pShaderResourceView_);
	_RELEASE(pRenderTargetView_);
	_RELEASE(pRenderTargetTexture_);
}



////////////////////////////////////////////////////////////////////////////////////////////
//                               PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

bool RenderToTextureClass::Initialize(ID3D11Device* pDevice,
	const UINT textureWidth,
	const UINT textureHeight,
	const float screenDepth,
	const float screenNear,
	const int format)
{
	// 1. this function will do the setup of the render texture object. The function creates
	//    a render target texture by first setting up the description of the texture and
	//    then creating that texture. 
	// 2. It then uses that texture to setup a render target view
	//    so that the texture can be drawn to as a render target. 
	// 3. The third thing we do is create a shader resource view of that texture so that 
	//    we can supply the rendered data to calling objects
	// 4. This function will also create a projection and ortho matrix for correct 
	//    perspective rendering of the render texture, since the dimensions of the render
	//    texture may vary. Remember to always keep the aspect ratio of this render
	//    to texture the same as the aspect ratio of where the texture will be used, 
	//    or there will be some size distortion.

	try
	{
	
		HRESULT hr = S_OK;
		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		DXGI_FORMAT textureFormat;

		ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
		ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		ZeroMemory(&textureFormat, sizeof(DXGI_FORMAT));

		//////////////////////////////////////////////////////////

		// set the texture format
		//textureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		// store the width and height of the render texture
		textureWidth_ = textureWidth;
		textureHeight_ = textureHeight;

		// setup the render target texture description
		textureDesc.Width = textureWidth;
		textureDesc.Height = textureHeight;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		// create the render target texture
		hr = pDevice->CreateTexture2D(&textureDesc, nullptr, &pRenderTargetTexture_);
		COM_ERROR_IF_FAILED(hr, "can't create the render target texture");

		//////////////////////////////////////////////////////////

		// setup the description of the render target view
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		// create the render target view
		hr = pDevice->CreateRenderTargetView(pRenderTargetTexture_, &renderTargetViewDesc, &pRenderTargetView_);
		COM_ERROR_IF_FAILED(hr, "can't create the render target view");

		//////////////////////////////////////////////////////////

		// setup the description of the shader resource view
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		// create the shader resource view
		hr = pDevice->CreateShaderResourceView(pRenderTargetTexture_, &shaderResourceViewDesc, &pShaderResourceView_);
		COM_ERROR_IF_FAILED(hr, "can't create the shader resource view");

		//////////////////////////////////////////////////////////

		// set up the description of the depth buffer
		depthBufferDesc.Width = textureWidth;
		depthBufferDesc.Height = textureHeight;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		// create the texture for the depth buffer using the filled out description
		hr = pDevice->CreateTexture2D(&depthBufferDesc, nullptr, &pDepthStencilBuffer_);
		COM_ERROR_IF_FAILED(hr, "can't create the texture for the depth buffer");

		//////////////////////////////////////////////////////////

		// set up the depth stencil view description
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		// create the depth stencil view
		hr = pDevice->CreateDepthStencilView(pDepthStencilBuffer_, &depthStencilViewDesc, &pDepthStencilView_);
		COM_ERROR_IF_FAILED(hr, "can't create the depth stencil view");

		//////////////////////////////////////////////////////////

		// Setup the viewport for rendering
		viewport_.Width    = static_cast<float>(textureWidth);
		viewport_.Height   = static_cast<float>(textureHeight);
		viewport_.MinDepth = 0.0f;
		viewport_.MaxDepth = 1.0f;
		viewport_.TopLeftX = 0;
		viewport_.TopLeftY = 0;

		float fTextureWidth  = static_cast<float>(textureWidth);
		float fTextureHeight = static_cast<float>(textureHeight);

		// setup the projection matrix
		projectionMatrix_ = XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4,
			fTextureWidth / fTextureHeight,     // aspect ratio
			screenNear,
			screenDepth);

		// create an orthographic projection matrix for 2D rendering
		orthoMatrix_ = XMMatrixOrthographicLH(fTextureWidth, fTextureHeight, screenNear, screenDepth);

	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize");
		return false;
	}

	return true;

} // end Initialize

//////////////////////////////////////////////////////////

void RenderToTextureClass::ChangeRenderTarget(ID3D11DeviceContext* pDeviceContext)
{
	// this function changes where we are currently rendering to. We are usually rendering
	// to the back buffer (or another render texture), but when we call this function,
	// we will now be rendering to this render texture.
	// Note that we also need to set the viewport for this render texture, since
	// the dimensions might be different that the back buffer or wherever we were
	// rendering to before this function was called

	// bind the render target view and depth stencil buffer to the output render pipeline
	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView_);

	// set the viewport
	pDeviceContext->RSSetViewports(1, &viewport_);

	return;

} // end ChangeRenderTarget

//////////////////////////////////////////////////////////

void RenderToTextureClass::ClearRenderTarget(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMFLOAT4 & rgbaColor)
{
	// this function is called right before rendering to this render texture so that
	// the texture and the depth buffer are cleared from their previous contents

	// setup the colour to clear the buffer to
	float color[4]{ rgbaColor.x, rgbaColor.y, rgbaColor.z, rgbaColor.w };

	// clear the back buffer
	pDeviceContext->ClearRenderTargetView(pRenderTargetView_, color);

	// clear the depth buffer
	pDeviceContext->ClearDepthStencilView(pDepthStencilView_, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;

} // end ClearRenderTarget

//////////////////////////////////////////////////////////

ID3D11ShaderResourceView** RenderToTextureClass::GetShaderResourceViewAddress()
{
	// this function gives us access to the texture for this render texture object.
	// Once the render texture has been rendered to the data of this texture is now
	// useable as a shader texture resource the same as any other TextureClass object

	return &pShaderResourceView_;
}

//////////////////////////////////////////////////////////

// the following four helper functions return the dimensions, the projection matrix,
// and ortho matrix for this render texture object
void RenderToTextureClass::GetProjectionMatrix(DirectX::XMMATRIX & projMatrix)
{
	projMatrix = projectionMatrix_;
	return;
}

//////////////////////////////////////////////////////////

void RenderToTextureClass::GetOrthoMatrix(DirectX::XMMATRIX & orthoMatrix)
{
	orthoMatrix = orthoMatrix_;
	return;
}

//////////////////////////////////////////////////////////

UINT RenderToTextureClass::GetTextureWidth() const
{
	return textureWidth_;
}

//////////////////////////////////////////////////////////

UINT RenderToTextureClass::GetTextureHeight() const
{
	return textureHeight_;
}



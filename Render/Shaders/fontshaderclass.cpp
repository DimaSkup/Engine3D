// ***********************************************************************************
// Filename: fontshaderclass.cpp
// Revising: 23.07.22
// ***********************************************************************************
#include "fontshaderclass.h"
#include "shaderclass.h"

#include "../Common/Log.h"
#include "../Common/Types.h"

namespace Render
{


FontShaderClass::FontShaderClass() : className_(__func__)
{
	//Log::Debug();
}

FontShaderClass::~FontShaderClass() 
{
}


// ***********************************************************************************
//
//                              PUBLIC FUNCTIONS
//
// ***********************************************************************************


bool FontShaderClass::Initialize(
	ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX& WVO)            // world * base_view * ortho
{
	// Initialize() initializes the vertex and pixel shaders, input layout,
	// sampler state, matrix and pixel buffers

	try
	{
		const std::wstring vsFilename = ShaderClass::pathToShadersDir_ + L"fontVS.hlsl";
		const std::wstring psFilename = ShaderClass::pathToShadersDir_ + L"fontPS.hlsl";

		// create shader objects, buffers, etc.
		InitializeShaders(pDevice,
			pDeviceContext,
			vsFilename.c_str(),
			psFilename.c_str(),
			WVO);
	}
	catch (LIB_Exception & e)
	{
		Log::Error(e, true);
		Log::Error("can't initialize the font shader class");
		return false;
	}

	return true;
}


// ************************************************************************************
//                             PUBLIC RENDERING API
// ************************************************************************************

void FontShaderClass::Render(
	ID3D11DeviceContext* pDeviceContext, 
	ID3D11ShaderResourceView* const* ppFontTexture,
	const std::vector<ID3D11Buffer*>& textVBs,    // array of text vertex buffers
	const std::vector<ID3D11Buffer*>& textIBs,    // array of text indices buffers
	const std::vector<uint32_t>& indexCounts,
	const uint32_t fontVertexSize)
{
	// THIS FUNC renders fonts on the screen using HLSL shaders

	try
	{
		// set vertex and pixel shaders for rendering
		pDeviceContext->VSSetShader(vs_.GetShader(), nullptr, 0U);
		pDeviceContext->PSSetShader(ps_.GetShader(), nullptr, 0U);

		// set the primitive topology for all the sentences and the input layout
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->IASetInputLayout(vs_.GetInputLayout());

		// set the sampler state for the pixel shader
		pDeviceContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());
		
		pDeviceContext->PSSetShaderResources(0, 1, ppFontTexture);

		// ------------------------------------------------
		// set up parameters for the vertex shader
		pDeviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());
		// set up parameters for the pixel shader
		pDeviceContext->PSSetConstantBuffers(0, 1, pixelBuffer_.GetAddressOf());


		const u32 numTextStrings = (u32)std::ssize(textVBs);
		Assert::True(numTextStrings == std::ssize(textIBs), "the number of vertex buffers must be equal to the number of index buffers");

		const UINT offset = 0;

		for (u32 idx = 0; idx < numTextStrings; ++idx)
		{
			// set the vertices and indices buffers as active
			pDeviceContext->IASetVertexBuffers(0, 1,
				&textVBs[idx],
				&fontVertexSize,
				&offset);

			pDeviceContext->IASetIndexBuffer(textIBs[idx], DXGI_FORMAT_R32_UINT, 0);


			// render the fonts on the screen
			pDeviceContext->DrawIndexed(indexCounts[idx], 0, 0);
		}

	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, true);
		throw LIB_Exception("can't render using the shader");
	}
}



// ************************************************************************************
//                             PUBLIC MODIFICATION API
// ************************************************************************************

void FontShaderClass::SetWorldViewOrtho(
	ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX& WVO)
{
	// prepare matrices for using in the vertex shader
	// (the WVO matrix must be already transposed)
	matrixBuffer_.data.worldViewProj = WVO;

	// load matrices data into GPU
	matrixBuffer_.ApplyChanges(pDeviceContext);

	// set up parameters for the vertex shader
	pDeviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());
}

///////////////////////////////////////////////////////////

void FontShaderClass::SetFontColor(
	ID3D11DeviceContext* pDeviceContext, 
	const DirectX::XMFLOAT3& textColor)
{
	// prepare data for the pixel shader
	pixelBuffer_.data.pixelColor = textColor;

	// load the pixel color data into GPU
	pixelBuffer_.ApplyChanges(pDeviceContext);

	// set up parameters for the pixel shader
	pDeviceContext->PSSetConstantBuffers(0, 1, pixelBuffer_.GetAddressOf());

}

///////////////////////////////////////////////////////////

void FontShaderClass::SetFontTexture(
	ID3D11DeviceContext* pDeviceContext,
	ID3D11ShaderResourceView* const* ppFontTexture)
{
	pDeviceContext->PSSetShaderResources(0, 1, ppFontTexture);
}




// ***********************************************************************************
//
//                              PRIVATE FUNCTIONS
//
// ***********************************************************************************

void FontShaderClass::InitializeShaders(
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext, 
	const WCHAR* vsFilename, 
	const WCHAR* psFilename,
	const DirectX::XMMATRIX& WVO)        // world * base_view * ortho
{
	// InitializeShaders() helps to initialize the vertex and pixel shaders,
	// input layout, sampler state, matrix and pixel buffers

	bool result = false;
	HRESULT hr = S_OK;
	const UINT layoutElemNum = 2;

	// a description of the vertex input layout
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum]; 

	// --------------------------  INPUT LAYOUT DESC  ---------------------------------

	// setup description of the vertex input layout 
	layoutDesc[0].SemanticName = "POSITION";
	layoutDesc[0].SemanticIndex = 0;
	layoutDesc[0].Format = DXGI_FORMAT_R32G32_FLOAT;
	layoutDesc[0].InputSlot = 0;
	layoutDesc[0].AlignedByteOffset = 0;
	layoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[0].InstanceDataStepRate = 0;

	layoutDesc[1].SemanticName = "TEXCOORD";
	layoutDesc[1].SemanticIndex = 0;
	layoutDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	layoutDesc[1].InputSlot = 0;
	layoutDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[1].InstanceDataStepRate = 0;


	// -----------------------  SHADERS / SAMPLER STATE  ------------------------------

	// initialize the vertex shader
	result = vs_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	Assert::True(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = ps_.Initialize(pDevice, psFilename);
	Assert::True(result, "can't initialize the pixel shader");

	// initialize the sampler state
	result = samplerState_.Initialize(pDevice);
	Assert::True(result, "can't initialize the sampler state");



	// ---------------------------  CONSTANT BUFFERS  ---------------------------------

	// initialize the matrix buffer
	hr = matrixBuffer_.Initialize(pDevice, pDeviceContext);
	Assert::NotFailed(hr, "can't initialize the matrix buffer");
	
	// initialize the pixel buffer
	hr = pixelBuffer_.Initialize(pDevice, pDeviceContext);
	Assert::NotFailed(hr, "can't initialize the pixel buffer");
	

	// ---------------- SET DEFAULT PARAMS FOR CONST BUFFERS --------------------------

	SetFontColor(pDeviceContext, { 1, 1, 1 });  // set white colour by default
	SetWorldViewOrtho(pDeviceContext, WVO);

	return;
}

}
/////////////////////////////////////////////////////////////////////
// Filename: colorshaderclass.cpp
// Revising: 06.04.22
/////////////////////////////////////////////////////////////////////
#include "colorshaderclass.h"


ColorShaderClass::ColorShaderClass()
	: className_ { __func__ }
{
	Log::Debug(LOG_MACRO);
}

ColorShaderClass::~ColorShaderClass()
{
}


// ------------------------------------------------------------------------------ //
//
//                         PUBLIC FUNCTIONS
//
// ------------------------------------------------------------------------------ //

bool ColorShaderClass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	// THIS FUNCTION initializes the ColorShaderClass; 
	// creates a vertex and pixel shader, input layout, and const buffer 

	try
	{
		const WCHAR* vsFilename = L"shaders/colorVS.hlsl";
		const WCHAR* psFilename = L"shaders/colorPS.hlsl";

		InitializeShaders(pDevice, pDeviceContext, vsFilename, psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the color shader class");
		return false;
	}

	Log::Debug(LOG_MACRO, "is initialized");

	return true;
}

///////////////////////////////////////////////////////////

void ColorShaderClass::RenderGeometry(ID3D11DeviceContext* pDeviceContext,
	ID3D11Buffer* vertexBufferPtr,
	ID3D11Buffer* indexBufferPtr,
	const UINT vertexBufferStride,
	const UINT indexCount,
	const std::vector<DirectX::XMMATRIX> & worldMatrices,
	const DirectX::XMMATRIX & viewProj,
	const float totalGameTime)            // time passed since the start of the application
{
	// THIS FUNCTION renders the input vertex buffer using the Color effect
	// and for painting vertices with some color we use its colors

	try
	{
		const UINT offset = 0;
		
		// -------------------------------------------------------------------------- //
		//         SETUP SHADER PARAMS WHICH ARE THE SAME FOR EACH MODEL              //
		// -------------------------------------------------------------------------- //

		// set the input layout for the vertex shader
		pDeviceContext->IASetInputLayout(vertexShader_.GetInputLayout());

		pDeviceContext->VSSetShader(vertexShader_.GetShader(), nullptr, 0U);
		pDeviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0U);

		// set a ptr to the vertex buffer and vertex buffer stride
		pDeviceContext->IASetVertexBuffers(0, 1,
			&vertexBufferPtr,
			&vertexBufferStride,
			&offset);

		// set a ptr to the index buffer
		pDeviceContext->IASetIndexBuffer(indexBufferPtr, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

		// setup constant buffer for shaders
		pDeviceContext->VSSetConstantBuffers(0, 1, constBuffPerObj_.GetAddressOf());

		// ------------------------------------------------------------------------------ //
		//    SETUP SHADER PARAMS WHICH ARE DIFFERENT FOR EACH MODEL AND RENDER MODELS    //
		// ------------------------------------------------------------------------------ //

		// go through each model, prepare it for rendering using the shader, and render it
		for (UINT idx = 0; idx < worldMatrices.size(); ++idx)
		{
			constBuffPerObj_.data.worldViewProj = DirectX::XMMatrixTranspose(worldMatrices[idx] * viewProj);

			const bool result = constBuffPerObj_.ApplyChanges(pDeviceContext);
			COM_ERROR_IF_FALSE(result, "can't update the const buffer");

			// draw geometry
			pDeviceContext->DrawIndexed(indexCount, 0, 0);
		} 
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't render geometry");
	}

	return;
}

///////////////////////////////////////////////////////////

void ColorShaderClass::RenderGeometry(ID3D11DeviceContext* pDeviceContext,
	ID3D11Buffer* vertexBufferPtr,
	ID3D11Buffer* indexBufferPtr,
	const std::vector<DirectX::XMFLOAT4> & colorsArr,
	const std::vector<DirectX::XMMATRIX> & worldMatrices,  // unique colour for each geometry obj
	const DirectX::XMMATRIX & viewProj,
	const float totalGameTime,                             // time passed since the start of the application
	const UINT vertexBufferStride,
	const UINT indexCount)
{
	// THIS FUNCTION renders each model with its unique color;
	// so sizes of the worldMatrices array and the colorsArr must be the same

	try
	{
		assert(worldMatrices.size() == colorsArr.size());

		const UINT offset = 0;

	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't render geometry");
	}


	return;
}

///////////////////////////////////////////////////////////

const std::string & ColorShaderClass::GetShaderName() const
{
	return className_;
}


// ------------------------------------------------------------------------------ //
//
//                         PRIVATE FUNCTIONS
//
// ------------------------------------------------------------------------------ //

// Initializes the shaders, input vertex layout and constant matrix buffer.
// This function is called from the Initialize() function
void ColorShaderClass::InitializeShaders(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	const WCHAR* vsFilename,
	const WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	bool result = false;
	const UINT layoutElemNum = 2;      // the number of the input layout elements
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum];

	// sum of the structures sizes of:
	// position (float3) + 
	// texture (float2) + 
	// normal (float3) +
	// tangent (float3) + 
	// binormal (float3);
	// (look at the the VERTEX structure)
	const UINT colorOffset = (4 * sizeof(DirectX::XMFLOAT3)) + sizeof(DirectX::XMFLOAT2);

	// set the description for the input layout
	layoutDesc[0].SemanticName = "POSITION";
	layoutDesc[0].SemanticIndex = 0;
	layoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[0].InputSlot = 0;
	layoutDesc[0].AlignedByteOffset = 0;
	layoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[0].InstanceDataStepRate = 0;

	layoutDesc[1].SemanticName = "COLOR";
	layoutDesc[1].SemanticIndex = 0;
	//layoutDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layoutDesc[1].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	layoutDesc[1].InputSlot = 0;
	layoutDesc[1].AlignedByteOffset = colorOffset;  
	layoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[1].InstanceDataStepRate = 0;


	// --------------------- SHADERS / SAMPLER STATE -------------------------- //

	// initialize the vertex shader
	result = vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the sky dome vertex shader");

	// initialize the pixel shader
	result = pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the sky dome pixel shader");

	// ------------------------ CONSTANT BUFFERS ------------------------------ //

	// initialize the constant buffer for data which is changed per object
	hr = constBuffPerObj_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the constant per object buffer");


	// --------------------- SETUP CONSTANT BUFFERS --------------------------- //

	// by default we use a color of vertex for painting
	constBuffPerObj_.data.isUseVertexColor = true;

	// but if we want to render with some particular color (by default white color)
	constBuffPerObj_.data.rgbColor = { 1.0f, 1.0f, 1.0f };

	return;
}

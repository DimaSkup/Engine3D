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

// Initializes the ColorShaderClass
bool ColorShaderClass::Initialize(ID3D11Device* pDevice, 
	                              ID3D11DeviceContext* pDeviceContext)
{
	try
	{
		const WCHAR* vsFilename = L"shaders/colorVertex.hlsl";
		const WCHAR* psFilename = L"shaders/colorPixel.hlsl";

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

bool ColorShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const UINT indexCount,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & viewProj,
	const DirectX::XMFLOAT4 & color)
{
	try
	{
		// set the shader parameters
		SetShaderParameters(pDeviceContext,
			world,
			viewProj,
			color);   // using this variable we can control both color and alpha value of the model

		// render the model using this shader
		RenderShader(pDeviceContext, indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't render the model");
	}

	return true;
} // end Render

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
	UINT colorOffset = (4 * sizeof(DirectX::XMFLOAT3)) + sizeof(DirectX::XMFLOAT2);


	// ---------------------------------------------------------------------------------- //
	//                         CREATION OF THE VERTEX SHADER                              //
	// ---------------------------------------------------------------------------------- //

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
	layoutDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layoutDesc[1].InputSlot = 0;
	layoutDesc[1].AlignedByteOffset = colorOffset;  
	layoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[1].InstanceDataStepRate = 0;


	// ---------------------------------- SHADERS --------------------------------------- //

	// initialize the vertex shader
	result = this->vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = this->pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the pixel shader");


	// ------------------------------- CONSTANT BUFFERS --------------------------------- //

	// initialize the matrix const buffer
	hr = matrixBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix buffer");

	// initialize the color const buffer
	hr = colorBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the color buffer");


	return;
} // InitializeShader()


// Setup parameters of shaders
// This function is called from the Render() function
void ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & viewProj,
	const DirectX::XMFLOAT4 & color)
{
	bool result = false; 

	// ----------------------- UPDATE THE VERTEX SHADER --------------------------------- //

	// update the matrix const buffer
	matrixBuffer_.data.world         = DirectX::XMMatrixTranspose(world);
	matrixBuffer_.data.worldViewProj = DirectX::XMMatrixTranspose(world * viewProj);

	result = matrixBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the matrix const buffer");
	
	// set the constant buffer for the vertex shader
	pDeviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());

	// update the color buffer;
	// PAY ATTENTION that in the HLSL shader we use all values of RGBA color which we
	// receive from the data container in the color variable;
	colorBuffer_.data.rgbaColor = color;
	
	result = colorBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the alpha color buffer");

	// set the constant buffer for the vertex shader
	pDeviceContext->VSSetConstantBuffers(1, 1, colorBuffer_.GetAddressOf());

	return;
}

// Sets as active the vertex and pixel shader, input vertex layout and matrix buffer
// Renders the model
// This function is called from the Render() function
void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, const UINT indexCount)
{
	// set shaders which will be used to render the model
	deviceContext->VSSetShader(vertexShader_.GetShader(), nullptr, 0);
	deviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0);

	// set the format of input shader data
	deviceContext->IASetInputLayout(vertexShader_.GetInputLayout());

	// render the model
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

/////////////////////////////////////////////////////////////////////
// Filename:   DepthShaderClass.cpp
// Created:    10.06.23
/////////////////////////////////////////////////////////////////////
#include "DepthShaderClass.h"


DepthShaderClass::DepthShaderClass(void)
{
	Log::Debug(THIS_FUNC_EMPTY);
	className_ = __func__;
}

DepthShaderClass::~DepthShaderClass(void)
{
}


// ------------------------------------------------------------------------------ //
//
//                         PUBLIC FUNCTIONS
//
// ------------------------------------------------------------------------------ //

// Initializes the ColorShaderClass
bool DepthShaderClass::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd)
{
	try
	{
		WCHAR* vsFilename = L"shaders/depthVertex.hlsl";
		WCHAR* psFilename = L"shaders/depthPixel.hlsl";

		bool result = InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
		COM_ERROR_IF_FALSE(result, "can't initialize shaders");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize the depth shader class");
		return false;
	}

	Log::Debug(THIS_FUNC, "is initialized");

	return true;
}


// Sets shaders parameters and renders our 3D model using HLSL shaders
bool DepthShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const UINT indexCount,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection)
{
	try
	{
		bool result = false;

		// set the shader parameters
		result = SetShaderParameters(pDeviceContext,
			world,                           
			view,
			projection);
		COM_ERROR_IF_FALSE(result, "can't set shader parameters");

		// render the model using this shader
		RenderShader(pDeviceContext, indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize the depth shader class");
		return false;
	}

	return true;
}



const std::string & DepthShaderClass::GetShaderName() const _NOEXCEPT
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
bool DepthShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd,
	WCHAR* vsFilename,
	WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	bool result = false;
	const UINT layoutElemNum = 1;      // the number of the input layout elements
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum];

	// ---------------------------------------------------------------------------------- //
	//                    CREATION OF THE VERTEX / PIXEL SHADER                           //
	// ---------------------------------------------------------------------------------- //

	// set the description for the input layout
	layoutDesc[0].SemanticName = "POSITION";
	layoutDesc[0].SemanticIndex = 0;
	layoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[0].InputSlot = 0;
	layoutDesc[0].AlignedByteOffset = 0;
	layoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[0].InstanceDataStepRate = 0;

	// initialize the vertex shader
	result = this->vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = this->pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the pixel shader");



	// ---------------------------------------------------------------------------------- //
	//                        CREATION OF CONSTANT BUFFERS                                //
	// ---------------------------------------------------------------------------------- //

	// initialize the matrix const buffer
	hr = matrixBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix buffer");


	return true;
} // InitializeShader()


  // Setup parameters of shaders
  // This function is called from the Render() function
bool DepthShaderClass::SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX & worldMatrix,
	const DirectX::XMMATRIX & viewMatrix,
	const DirectX::XMMATRIX & projectionMatrix)
{
	bool result = false;

	// update the matrix const buffer
	matrixBuffer_.data.world = DirectX::XMMatrixTranspose(worldMatrix);
	matrixBuffer_.data.view = DirectX::XMMatrixTranspose(viewMatrix);
	matrixBuffer_.data.projection = DirectX::XMMatrixTranspose(projectionMatrix);

	result = matrixBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the matrix const buffer");

	pDeviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());

	return true;
}

// Sets as active the vertex and pixel shader, input vertex layout and matrix buffer
// Renders the model
// This function is called from the Render() function
void DepthShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, const UINT indexCount)
{
	// set shaders which will be used to render the model
	deviceContext->VSSetShader(vertexShader_.GetShader(), nullptr, 0);
	deviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0);

	// set the format of input shader data
	deviceContext->IASetInputLayout(vertexShader_.GetInputLayout());

	// set the input shader data (constant buffer)
	deviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());

	// render the model
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

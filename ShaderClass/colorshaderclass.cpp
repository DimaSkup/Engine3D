/////////////////////////////////////////////////////////////////////
// Filename: colorshaderclass.cpp
// Revising: 06.04.22
/////////////////////////////////////////////////////////////////////
#include "colorshaderclass.h"


ColorShaderClass::ColorShaderClass(void)
{
	Log::Debug(THIS_FUNC_EMPTY);
	className_ = __func__;

	pMatrixBuffer_ = std::make_unique<ConstantBuffer<ConstantMatrixBuffer_VS>>();
	pColorBuffer_  = std::make_unique<ConstantBuffer<ConstantColorBuffer_VS>>();
}


ColorShaderClass::~ColorShaderClass(void)
{
}


// ------------------------------------------------------------------------------ //
//
//                         PUBLIC FUNCTIONS
//
// ------------------------------------------------------------------------------ //

// Initializes the ColorShaderClass
bool ColorShaderClass::Initialize(ID3D11Device* pDevice, 
	                              ID3D11DeviceContext* pDeviceContext,
	                              HWND hwnd)
{
	try
	{
		WCHAR* vsFilename = L"shaders/colorVertex.hlsl";
		WCHAR* psFilename = L"shaders/colorPixel.hlsl";

		InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize the color shader class");
		return false;
	}

	Log::Debug(THIS_FUNC, "is initialized");

	return true;
}

///////////////////////////////////////////////////////////

bool ColorShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	DataContainerForShaders* pDataForShader)
{
	//Log::Print(THIS_FUNC_EMPTY);

	assert(pDataForShader != nullptr);

	try
	{
		// set the shader parameters
		SetShaderParameters(pDeviceContext,
			pDataForShader->world,
			pDataForShader->view,
			pDataForShader->orthoOrProj,
			pDataForShader->modelColor);

		// render the model using this shader
		RenderShader(pDeviceContext, pDataForShader->indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't render the model");
	}

	return true;
} // end Render

///////////////////////////////////////////////////////////


// Sets shaders parameters and renders our model using HLSL shaders
bool ColorShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const int indexCount,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	const DirectX::XMFLOAT4 & color)
{
	try
	{
		// set the shader parameters
		SetShaderParameters(pDeviceContext,
			world,                           
			view,
			projection,
			color);

		// render the model using this shader
		RenderShader(pDeviceContext, indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't render the model");
	}

	return true;
}



const std::string & ColorShaderClass::GetShaderName() const _NOEXCEPT
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
	HWND hwnd,
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
	hr = pMatrixBuffer_->Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix buffer");

	// initialize the color const buffer
	hr = pColorBuffer_->Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the color buffer");


	return;
} // InitializeShader()


// Setup parameters of shaders
// This function is called from the Render() function
void ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	const DirectX::XMFLOAT4 & color)
{
	bool result = false; 

	// ----------------------- UPDATE THE VERTEX SHADER --------------------------------- //

	// update the matrix const buffer
	pMatrixBuffer_->data.world      = DirectX::XMMatrixTranspose(world);
	pMatrixBuffer_->data.view       = DirectX::XMMatrixTranspose(view);
	pMatrixBuffer_->data.projection = DirectX::XMMatrixTranspose(projection);

	result = pMatrixBuffer_->ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the matrix const buffer");
	
	// set the constant buffer for the vertex shader
	pDeviceContext->VSSetConstantBuffers(0, 1, pMatrixBuffer_->GetAddressOf());

	// update the color buffer
	pColorBuffer_->data.red   = color.x;
	pColorBuffer_->data.green = color.y;
	pColorBuffer_->data.blue  = color.z;
	pColorBuffer_->data.alpha = color.w;
	
	result = pColorBuffer_->ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the color buffer");

	// set the constant buffer for the vertex shader
	pDeviceContext->VSSetConstantBuffers(1, 1, pColorBuffer_->GetAddressOf());


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

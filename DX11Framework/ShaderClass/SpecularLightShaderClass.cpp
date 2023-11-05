////////////////////////////////////////////////////////////////////
// Filename: SpecularLightShaderClass.cpp
////////////////////////////////////////////////////////////////////
#include "SpecularLightShaderClass.h"

SpecularLightShaderClass::SpecularLightShaderClass(void)
{
	Log::Debug(THIS_FUNC_EMPTY);
	className_ = __func__;
}

SpecularLightShaderClass::~SpecularLightShaderClass(void) 
{
}



// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PUBLIC FUNCTIONS                                         //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// Initializes the shaders for rendering of the model
bool SpecularLightShaderClass::Initialize(ID3D11Device* pDevice, 
	                              ID3D11DeviceContext* pDeviceContext, 
	                              HWND hwnd)
{
	try
	{
		const WCHAR* vsFilename = L"shaders/specularLightVertex.hlsl";
		const WCHAR* psFilename = L"shaders/specularLightPixel.hlsl";

		// try to initialize the vertex and pixel HLSL shaders
		InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize the specular light shader class");
		return false;
	}

	Log::Debug(THIS_FUNC, "is initialized");

	return true;
}


// 1. Sets the parameters for HLSL shaders which are used for rendering
// 2. Renders the model using the HLSL shaders
bool SpecularLightShaderClass::Render(ID3D11DeviceContext* deviceContext,
	const UINT indexCount,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	ID3D11ShaderResourceView* const* pTextureArray,
	const DirectX::XMFLOAT3 & cameraPosition,
	LightClass* pLightSources)
{
	try
	{
		// set the shader parameters
		SetShaderParameters(deviceContext,
			world,
			view,
			projection,
			pTextureArray,
			cameraPosition,
			pLightSources);

		// render the model using this shader
		RenderShader(deviceContext, indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't render using the shader");
		return false;
	}

	return true;
}


const std::string & SpecularLightShaderClass::GetShaderName() const _NOEXCEPT
{
	return className_;
}


// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PRIVATE FUNCTIONS                                        //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// helps to initialize the HLSL shaders, layout, sampler state, and buffers
void SpecularLightShaderClass::InitializeShaders(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd, 
	const WCHAR* vsFilename, 
	const WCHAR* psFilename)
{
	bool result = false;
	HRESULT hr = S_OK;
	const UINT layoutElemNum = 3;                       // the number of the input layout elements
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum]; // description for the vertex input layout


	// set the description for the input layout
	layoutDesc[0].SemanticName = "POSITION";
	layoutDesc[0].SemanticIndex = 0;
	layoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
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

	layoutDesc[2].SemanticName = "NORMAL";
	layoutDesc[2].SemanticIndex = 0;
	layoutDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[2].InputSlot = 0;
	layoutDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[2].InstanceDataStepRate = 0;


	// -------------------------- SHADERS / SAMPLER STATE ------------------------------- //

	// initialize the vertex shader
	result = this->vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = this->pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the pixel shader");

	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the sampler state");


	// ----------------------------- CONSTANT BUFFERS ----------------------------------- //

	// initialize the constant matrix buffer
	hr = this->matrixBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix buffer");

	// initialize the constnat light buffer
	hr = this->lightBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the light buffer");

	// initialize the constant camera buffer
	hr = this->cameraBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the camera buffer");

	return;
} // InitializeShaders()



// sets parameters for the HLSL shaders
void SpecularLightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	ID3D11ShaderResourceView* const* pTextureArray,        
	const DirectX::XMFLOAT3 & cameraPosition,
	LightClass* pLightSources)
{
	bool result = false;
	
	// ---------------------------------------------------------------------------------- //
	//                     UPDATE BUFFERS FOR THE VERTEX SHADER                           //
	// ---------------------------------------------------------------------------------- //

	// prepare matrices for using in the HLSL constant matrix buffer
	matrixBuffer_.data.world      = DirectX::XMMatrixTranspose(world);
	matrixBuffer_.data.view       = DirectX::XMMatrixTranspose(view);
	matrixBuffer_.data.projection = DirectX::XMMatrixTranspose(projection);

	// update the constant matrix buffer
	result = matrixBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the matrix buffer");
	
	// set the buffer for the vertex shader
	deviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());



	// prepare data for the constant camera buffer
	cameraBuffer_.data.cameraPosition = cameraPosition;

	// update the constant camera buffer
	result = cameraBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the camera buffer");

	// set the buffer for the vertex shader
	deviceContext->VSSetConstantBuffers(1, 1, cameraBuffer_.GetAddressOf());



	// ---------------------------------------------------------------------------------- //
	//                     UPDATE BUFFERS FOR THE PIXEL SHADER                            //
	// ---------------------------------------------------------------------------------- //

	// write data into the buffer
	lightBuffer_.data.diffuseColor   = pLightSources->GetDiffuseColor();
	lightBuffer_.data.lightDirection = pLightSources->GetDirection();
	lightBuffer_.data.ambientColor   = pLightSources->GetAmbientColor();
	lightBuffer_.data.specularColor  = pLightSources->GetSpecularColor();
	lightBuffer_.data.specularPower  = pLightSources->GetSpecularPower();

	// update the constant camera buffer
	result = lightBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the light buffer");

	// set the constant light buffer for the HLSL pixel shader
	deviceContext->PSSetConstantBuffers(0, 1, lightBuffer_.GetAddressOf());

	// set the shader resource for the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &pTextureArray[0]);


	return;
} // SetShaderParameters


// sets stuff which we will use: layout, vertex and pixel shader, sampler state
// and also renders our 3D model
void SpecularLightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, 
	const UINT indexCount)
{
	// set the input layout for the vertex shader
	deviceContext->IASetInputLayout(vertexShader_.GetInputLayout());

	// set shader which we will use for rendering
	deviceContext->VSSetShader(vertexShader_.GetShader(), nullptr, 0);
	deviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0);

	// set the sampler state for the pixel shader
	deviceContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

	// render the model
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
} // RenderShader
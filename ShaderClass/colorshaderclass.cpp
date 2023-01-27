/////////////////////////////////////////////////////////////////////
// Filename: colorshaderclass.cpp
// Revising: 06.04.22
/////////////////////////////////////////////////////////////////////
#include "colorshaderclass.h"

ColorShaderClass::ColorShaderClass(void)
{
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& another) 
{
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
	bool result = false;
	WCHAR* vsFilename = L"shaders/colorVertex.hlsl";
	WCHAR* psFilename = L"shaders/colorPixel.hlsl";

	result = InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize shaders");

	Log::Debug(THIS_FUNC, "ColorShaderClass is initialized successfully");

	return true;
}


// Sets shaders parameters and renders our 3D model using HLSL shaders
bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	                          DirectX::XMMATRIX worldMatrix,
	                          DirectX::XMMATRIX viewMatrix,
	                          DirectX::XMMATRIX projectionMatrix)
{
	bool result = false;

	// set the shader parameters
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	COM_ERROR_IF_FALSE(result, "can't set shader parameters");

	// render the model using this shader
	RenderShader(deviceContext, indexCount);

	return true;
}



// memory allocation
void* ColorShaderClass::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);
	if (!ptr)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for object");
		return nullptr;
	}

	return ptr;
}

void ColorShaderClass::operator delete(void* p)
{
	_aligned_free(p);
}

// ------------------------------------------------------------------------------ //
//
//                         PRIVATE FUNCTIONS
//
// ------------------------------------------------------------------------------ //

// Initializes the shaders, input vertex layout and constant matrix buffer.
// This function is called from the Initialize() function
bool ColorShaderClass::InitializeShaders(ID3D11Device* pDevice, 
	                                     ID3D11DeviceContext* pDeviceContext,
	                                     HWND hwnd,
	                                     WCHAR* vsFilename,
                                         WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	bool result = false;
	const UINT layoutElemNum = 4;      // the number of the input layout elements
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum];
	

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

	layoutDesc[3].SemanticName = "COLOR";
	layoutDesc[3].SemanticIndex = 0;
	layoutDesc[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layoutDesc[3].InputSlot = 0;
	layoutDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[3].InstanceDataStepRate = 0;


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
bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
	                                       DirectX::XMMATRIX worldMatrix,
	                                       DirectX::XMMATRIX viewMatrix,
	                                       DirectX::XMMATRIX projectionMatrix)
{
	bool result = false; 

	// update the matrix const buffer
	matrixBuffer_.data.world      = DirectX::XMMatrixTranspose(worldMatrix);
	matrixBuffer_.data.view       = DirectX::XMMatrixTranspose(viewMatrix);
	matrixBuffer_.data.projection = DirectX::XMMatrixTranspose(projectionMatrix);

	result = matrixBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the matrix const buffer");
	
	pDeviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());

	return true;
}

// Sets as active the vertex and pixel shader, input vertex layout and matrix buffer
// Renders the model
// This function is called from the Render() function
void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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

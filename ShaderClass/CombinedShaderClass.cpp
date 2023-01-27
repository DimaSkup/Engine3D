////////////////////////////////////////////////////////////////////
// Filename: CombinedShaderClass.cpp
////////////////////////////////////////////////////////////////////
#include "CombinedShaderClass.h"

CombinedShaderClass::CombinedShaderClass(void)
{
	Log::Debug(THIS_FUNC_EMPTY);
}

// we don't use the copy constructor and destructor in this class
CombinedShaderClass::CombinedShaderClass(const CombinedShaderClass& anotherObj) {}
CombinedShaderClass::~CombinedShaderClass(void) {}



// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PUBLIC FUNCTIONS                                         //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// Initializes the shaders for rendering of the model
bool CombinedShaderClass::Initialize(ID3D11Device* pDevice,
								ID3D11DeviceContext* pDeviceContext,
								HWND hwnd)
{
	Log::Debug(THIS_FUNC_EMPTY);

	bool result = false;
	WCHAR* vsFilename = L"shaders/combinedVS.hlsl";
	WCHAR* psFilename = L"shaders/combinedPS.hlsl";

	// try to initialize the vertex and pixel HLSL shaders
	result = InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize shaders");

	return true;
}


// 1. Sets the parameters for HLSL shaders which are used for rendering
// 2. Renders the model using the HLSL shaders
bool CombinedShaderClass::Render(ID3D11DeviceContext* deviceContext,
							ModelClass* pModelToRender,
							const DirectX::XMMATRIX & view,
							const DirectX::XMMATRIX & projection,
							const DirectX::XMFLOAT3 & cameraPosition,
							const LightClass* pLight)
							//DirectX::XMFLOAT4 diffuseColor,       // a main directed colour (this colour and texture pixel colour are blending and make a final texture pixel colour of the model)
							//DirectX::XMFLOAT3 lightDirection,     // a direction of the diffuse colour
							//DirectX::XMFLOAT4 ambientColor,       // a common colour for the scene
							//DirectX::XMFLOAT3 cameraPosition,     // the current position of the camera
							//DirectX::XMFLOAT4 specularColor,      // the specular colour is the reflected colour of the object's highlights
							//float specularPower)                  // specular intensity
{
	bool result = false;

	// set the shader parameters
	result = SetShaderParameters(deviceContext,
								 pModelToRender->GetWorldMatrix(),
								 view, projection,
								 pModelToRender->GetTextureArray(),
								 cameraPosition,
								 pLight);
	COM_ERROR_IF_FALSE(result, "can't set the shader parameters");


	// render the model using this shader
	RenderShader(deviceContext, pModelToRender->GetIndexCount());

	return true;
}



// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PRIVATE FUNCTIONS                                        //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// helps to initialize the HLSL shaders, layout, sampler state, and buffers
bool CombinedShaderClass::InitializeShaders(ID3D11Device* pDevice,
									   ID3D11DeviceContext* pDeviceContext,
									   HWND hwnd,
									   WCHAR* vsFilename,
									   WCHAR* psFilename)
{
	Log::Debug(THIS_FUNC_EMPTY);

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


	// initialize the vertex shader
	if (!this->vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum))
		return false;


	// initialize the pixel shader
	if (!this->pixelShader_.Initialize(pDevice, psFilename))
		return false;


	// initialize the sampler state
	if (!this->samplerState_.Initialize(pDevice))
		return false;


	// initialize the constant matrix buffer
	hr = this->matrixBuffer_.Initialize(pDevice, pDeviceContext);
	if (FAILED(hr))
		return false;

	// initialize the constnat light buffer
	hr = this->lightBuffer_.Initialize(pDevice, pDeviceContext);
	if (FAILED(hr))
		return false;

	// initialize the constant camera buffer
	hr = this->cameraBuffer_.Initialize(pDevice, pDeviceContext);
	if (FAILED(hr))
		return false;

	return true;
} // InitializeShaders()



  // sets parameters for the HLSL shaders
bool CombinedShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
										 const DirectX::XMMATRIX & world,
										 const DirectX::XMMATRIX & view,
										 const DirectX::XMMATRIX & projection,
										 ID3D11ShaderResourceView** textureArray,  // a texture resource for the model
										 const DirectX::XMFLOAT3 & cameraPosition,
										 const LightClass* pLight)
{
	HRESULT hr = S_OK;
	UINT bufferPosition = 0;
	UINT texturesNumber = 3;

	// ---------------------------------------------------------------------------------- //
	//                         UPDATE THE VERTEX SHADER                                   //
	// ---------------------------------------------------------------------------------- //

	// update the constant matrix buffer
	matrixBuffer_.data.world      = DirectX::XMMatrixTranspose(world);
	matrixBuffer_.data.view       = DirectX::XMMatrixTranspose(view);
	matrixBuffer_.data.projection = DirectX::XMMatrixTranspose(projection);

	if (!matrixBuffer_.ApplyChanges())
		return false;

	// set the buffer position
	bufferPosition = 0;

	// set the matrix const buffer for the vertex shader
	deviceContext->VSSetConstantBuffers(bufferPosition, 1, matrixBuffer_.GetAddressOf());



	// update the constant camera buffer
	cameraBuffer_.data.cameraPosition = cameraPosition;
	cameraBuffer_.data.padding = 0.0f;

	if (!cameraBuffer_.ApplyChanges())
		return false;

	// set the buffer position in the vertex shader
	bufferPosition = 1;  

	// set the camera const buffer for the vertex shader
	deviceContext->VSSetConstantBuffers(bufferPosition, 1, cameraBuffer_.GetAddressOf());





	// ---------------------------------------------------------------------------------- //
	//                          UPDATE THE PIXEL SHADER                                   //
	// ---------------------------------------------------------------------------------- //

	// update the light const buffer
	lightBuffer_.data.diffuseColor = pLight->GetDiffuseColor();
	lightBuffer_.data.lightDirection = pLight->GetDirection();
	lightBuffer_.data.ambientColor = pLight->GetAmbientColor();
	lightBuffer_.data.specularColor = pLight->GetSpecularColor();
	lightBuffer_.data.specularPower = pLight->GetSpecularPower();

	if (!lightBuffer_.ApplyChanges())
		return false;

	// set the buffer position in the pixel shader
	bufferPosition = 0;

	// set the light const buffer for the HLSL pixel shader
	deviceContext->PSSetConstantBuffers(bufferPosition, 1, lightBuffer_.GetAddressOf());

	// set the shader resource (textures) for the pixel shader
	deviceContext->PSSetShaderResources(0, texturesNumber, textureArray);


	return true;
} // SetShaderParameters


  // sets stuff which we will use: layout, vertex and pixel shader, sampler state
  // and also renders our 3D model
void CombinedShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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
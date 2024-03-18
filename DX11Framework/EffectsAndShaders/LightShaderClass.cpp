////////////////////////////////////////////////////////////////////
// Filename:     LightShaderClass.cpp
// Description:  this class is needed for rendering 3D models, 
//               its texture, simple DIFFUSE light on it using HLSL shaders.
// Created:      09.04.23
////////////////////////////////////////////////////////////////////
#include "LightShaderClass.h"

LightShaderClass::LightShaderClass()
	: className_{ __func__ }
{
	Log::Debug(LOG_MACRO);
}

LightShaderClass::~LightShaderClass(void)
{
}



// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PUBLIC FUNCTIONS                                         //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// Initializes the shaders for rendering of the model
bool LightShaderClass::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	try
	{
		const WCHAR* vsFilename = L"shaders/lightVertex.hlsl";
		const WCHAR* psFilename = L"shaders/lightPixel.hlsl";

		// try to initialize the vertex and pixel HLSL shaders
		InitializeShaders(pDevice, pDeviceContext, vsFilename, psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the light shader class");
		return false;
	}

	Log::Debug(LOG_MACRO, "is initialized");

	return true;
} // end Initialize

///////////////////////////////////////////////////////////

bool LightShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const LightSourceDiffuseStore & diffuseLights,
	const std::vector<DirectX::XMMATRIX> & worldMatrices,                     // each model has its own world matrix
	const DirectX::XMMATRIX & viewProj,                                       // common view_matrix * proj_matrix
	const DirectX::XMFLOAT3 & cameraPosition,
	const DirectX::XMFLOAT3 & fogColor,
	const std::vector<ID3D11ShaderResourceView* const*> & ppDiffuseTextures,  // from the perspective of this shader each model has only one diffuse texture
	ID3D11Buffer* pVertexBuffer,
	ID3D11Buffer* pIndexBuffer,
	const UINT vertexBufferStride,
	const UINT indexCount,
	const float fogStart,
	const float fogRange,
	const bool  fogEnabled)
{
	// we call this function from the model_to_shader mediator

	try
	{
		const UINT offset = 0;
		bool result = false;


		// ---------------------------------------------------------------------------------- //
		//               SETUP SHADER PARAMS WHICH ARE THE SAME FOR EACH MODEL                //
		// ---------------------------------------------------------------------------------- //

		// set stuff which we will use: layout, vertex and pixel shader,
		// sampler state

		// set the input layout for the vertex shader
		pDeviceContext->IASetInputLayout(vertexShader_.GetInputLayout());

		// set shader which we will use for rendering
		pDeviceContext->VSSetShader(vertexShader_.GetShader(), nullptr, 0);
		pDeviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0);

		// set the sampler state for the pixel shader
		pDeviceContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

		// set a ptr to the vertex buffer and vertex buffer stride
		pDeviceContext->IASetVertexBuffers(0, 1,
			&pVertexBuffer,
			&vertexBufferStride,
			&offset);

		// set a ptr to the index buffer
		pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);


		// ------------ PIXEL SHADER: UPDATE THE CONSTANT LIGHT BUFFER ------------ //

		// write data into the buffer
		DirectX::XMFLOAT3 lightDir;
		DirectX::XMStoreFloat3(&lightDir, diffuseLights.directions_[0]);

		lightBuffer_.data.diffuseColor = diffuseLights.diffuseColors_[0];
		lightBuffer_.data.diffuseLightStrenght = diffuseLights.diffusePowers_[0];
		lightBuffer_.data.lightDirection = lightDir;
		lightBuffer_.data.ambientColor = diffuseLights.ambientColors_[0];
		lightBuffer_.data.ambientLightStrength = 1.0f;

		// update the constant camera buffer
		result = lightBuffer_.ApplyChanges(pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't update the light buffer");

		// set the constant light buffer for the HLSL pixel shader
		pDeviceContext->PSSetConstantBuffers(0, 1, lightBuffer_.GetAddressOf());


		// ------------ PIXEL SHADER: UPDATE THE CONSTANT CAMERA BUFFER ------------ //

		// prepare data for the constant camera buffer
		cameraBuffer_.data.cameraPosition = cameraPosition;
		cameraBuffer_.data.padding = 0.0f;

		// update the constant camera buffer
		result = cameraBuffer_.ApplyChanges(pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't update the camera buffer");

		// set the buffer for the vertex shader
		pDeviceContext->PSSetConstantBuffers(1, 1, cameraBuffer_.GetAddressOf());


		// ------------ PIXEL SHADER: UPDATE THE BUFFER PER FRAME ------------ //

		// only if fog enabled we update its params
		if (bufferPerFrame_.data.fogEnabled = fogEnabled)
		{

			bufferPerFrame_.data.fogColor = fogColor;
			bufferPerFrame_.data.fogStart = fogStart;
			bufferPerFrame_.data.fogRange = fogRange;
		}

		// setup if we want to use normal vector values as a colour of the pixel
		bufferPerFrame_.data.debugNormals = false;

		// update the constant camera buffer
		result = bufferPerFrame_.ApplyChanges(pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't update the buffer per frame");

		// set the constant light buffer for the HLSL pixel shader
		pDeviceContext->PSSetConstantBuffers(2, 1, bufferPerFrame_.GetAddressOf());


		// ------------  PIXEL SHADER: UPDATE SHADER TEXTURE RESOURCES  ------------ //

		// set textures for the pixel shader
		pDeviceContext->PSSetShaderResources(0, 1, ppDiffuseTextures[0]);

		// ---------------------------------------------------------------------------------- //
		//               SETUP SHADER PARAMS WHICH ARE DIFFERENT FOR EACH MODEL               //
		// ---------------------------------------------------------------------------------- //

		// go through each model, prepare it for rendering, and use the shader to render it
		for (UINT idx = 0; idx < worldMatrices.size(); ++idx)
		{

			// ------------ VERTEX SHADER: UPDATE THE CONSTANT MATRIX BUFFER ------------ //

			// prepare matrices for using in the HLSL constant matrix buffer
			matrixBuffer_.data.world = DirectX::XMMatrixTranspose(worldMatrices[idx]);
			matrixBuffer_.data.worldViewProj = DirectX::XMMatrixTranspose(worldMatrices[idx] * viewProj);

			// update the constant matrix buffer
			result = matrixBuffer_.ApplyChanges(pDeviceContext);
			COM_ERROR_IF_FALSE(result, "can't update the matrix buffer");

			// set the buffer for the vertex shader
			pDeviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());

			// draw geometry
			pDeviceContext->DrawIndexed(indexCount, 0, 0);
		}

	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't render");
		return false;
	}

	return true;

} // end Render

///////////////////////////////////////////////////////////


const std::string & LightShaderClass::GetShaderName() const
{
	return className_;
}


// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PRIVATE FUNCTIONS                                        //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// helps to initialize the HLSL shaders, layout, sampler state, and buffers
void LightShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
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
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");


	// ----------------------------- CONSTANT BUFFERS ----------------------------------- //

	// initialize the constant matrix buffer
	hr = this->matrixBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix buffer");

	// initialize the constant light buffer
	hr = this->lightBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the light buffer");

	// initialize the constant camera buffer
	hr = this->cameraBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the camera buffer");

	// initialize the constant buffer per frame
	hr = this->bufferPerFrame_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the buffer per frame");

	return;
}

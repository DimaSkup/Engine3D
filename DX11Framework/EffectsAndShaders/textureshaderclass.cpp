////////////////////////////////////////////////////////////////////
// Filename: textureshaderclass.cpp
////////////////////////////////////////////////////////////////////
#include "textureshaderclass.h"



TextureShaderClass::TextureShaderClass(void) 
	: className_{__func__}
{
	Log::Debug(LOG_MACRO);
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass&)
{
}

TextureShaderClass::~TextureShaderClass(void) 
{
}


// ------------------------------------------------------------------------- //
//
//                        PUBLIC METHODS
//
// ------------------------------------------------------------------------- //

// Loads the texture HLSL files for this shader
bool TextureShaderClass::Initialize(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext)
{
	try
	{
		const WCHAR* vsFilename = L"shaders/textureVertex.hlsl";
		const WCHAR* psFilename = L"shaders/texturePixel.hlsl";

		InitializeShaders(pDevice, 
			pDeviceContext, 
			vsFilename,
			psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the texture shader class");
		return false;
	}

	Log::Debug(LOG_MACRO, "is initialized");

	return true;
}

///////////////////////////////////////////////////////////

bool TextureShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const std::vector<DirectX::XMMATRIX> & worldMatrices,                     // each model has its own world matrix
	const DirectX::XMMATRIX & viewProj,                                       // common view_matrix * proj_matrix
	const DirectX::XMFLOAT3 & cameraPosition,
	const DirectX::XMFLOAT3 & fogColor,
	const std::vector<ID3D11ShaderResourceView* const*> & ppDiffuseTextures,  // from the perspective of this shader each model has only one diffuse texture
	ID3D11Buffer* pVertexBuffer,
	ID3D11Buffer* pIndexBuffer,
	const UINT vertexBufferStride,
	const UINT indexCount,
	//const std::vector<ID3D11Buffer*> & vertexBuffersPtrs,
	//const std::vector<ID3D11Buffer*> & indexBuffersPtrs,
	//const std::vector<UINT> & vertexBuffersStrides,
	//const std::vector<UINT> & indexCounts,
	//const UINT numOfModels,
	const float fogStart,
	const float fogRange,
	const bool  fogEnabled,
	const bool  useAlphaClip)
{

	try
	{
		// Sets the variables which are used within the vertex shader.
		// This function takes in a pointer to a texture resource and then assigns it to 
		// the shader using a texture resource pointer. Note that the texture has to be set 
		// before rendering of the buffer occurs.
		//
		// After all the preparations we render the model using the HLSL shader

		bool result = false;

		const AddressesOfMembers addresses = addresses_;

		// setup pipeline params which are the same for each model during this rendering
		this->PrepareShadersForRendering(pDeviceContext,
			pVertexBuffer,
			pIndexBuffer,
			addresses,
			cameraPosition,
			fogColor,
			vertexBufferStride,
			fogStart,
			fogRange,
			fogEnabled,
			useAlphaClip);

		// ---------------------------------------------------------------------
		//                     PIXEL SHADER: SET TEXTURES
		// ---------------------------------------------------------------------
		pDeviceContext->PSSetShaderResources(0, 1, ppDiffuseTextures[0]);


		// -------------------------------------------------------------------------
		//          SETUP SHADER PARAMS WHICH ARE DIFFERENT FOR EACH MODEL
		// -------------------------------------------------------------------------

		for (UINT idx = 0; idx < worldMatrices.size(); ++idx)
		{
			// ---------------------------------------------------------------------
			//          VERTEX SHADER: UPDATE THE CONSTANT MATRIX BUFFER
			// ---------------------------------------------------------------------

			// update data of the matrix const buffer
			matrixConstBuffer_.data.world = DirectX::XMMatrixTranspose(worldMatrices[idx]);
			matrixConstBuffer_.data.worldViewProj = DirectX::XMMatrixTranspose(worldMatrices[idx] * viewProj);

			// load matrices data into GPU
			matrixConstBuffer_.ApplyChanges(pDeviceContext);

			// set the matrix const buffer in the vertex shader with the updated values
			pDeviceContext->VSSetConstantBuffers(0, 1, addresses.matrixConstBufferAddress);

			// Render the model
			pDeviceContext->DrawIndexed(indexCount, 0, 0);

		} // end for
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't render the model");
		return false;
	}


	return true;
}

///////////////////////////////////////////////////////////


const std::string & TextureShaderClass::GetShaderName() const
{
	return className_;
}











////////////////////////////////////////////////////////////////////////////////////////////////
//                            PRIVATE MODIFICATION API
////////////////////////////////////////////////////////////////////////////////////////////////

// initialized the vertex shader, pixel shader, input layout, and sampler;
void TextureShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const WCHAR* vsFilename, 
	const WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	bool result = false;
	const UINT layoutElemNum = 2;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[layoutElemNum];


	// --------------------------- INPUT LAYOUT DESC -------------------------------

	// Create the vertex input layout description
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;


	// ------------------------  SHADERS / SAMPLER STATE  --------------------------

	// initialize the vertex shader
	result = vertexShader_.Initialize(pDevice, vsFilename, polygonLayout, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the pixel shader");

	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the sampler state");



	// ---------------------------  CONSTANT BUFFERS  ------------------------------

	// initialize the matrix const buffer (for the vertex shader)
	hr = this->matrixConstBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix const buffer");

	// initialize the constant camera buffer
	hr = this->cameraBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the camera buffer");

	// initialize the buffer per frame
	hr = this->bufferPerFrame_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the buffer per frame");


	// ----------------------  GET ADDRESSES OF MEMBERS  ---------------------------

	// later we will use all these addresses during rendering
	addresses_.pVertexShader = vertexShader_.GetShader();
	addresses_.pVertexShaderInputLayout = vertexShader_.GetInputLayout();
	addresses_.pPixelShader = pixelShader_.GetShader();
	addresses_.ppSamplerState = samplerState_.GetAddressOf();
	
	addresses_.matrixConstBufferAddress = matrixConstBuffer_.GetAddressOf();
	addresses_.cameraConstBufferAddress = cameraBuffer_.GetAddressOf();
	addresses_.bufferPerFrameAddress = bufferPerFrame_.GetAddressOf();

	return;
}

///////////////////////////////////////////////////////////

void TextureShaderClass::PrepareShadersForRendering(
	ID3D11DeviceContext* pDeviceContext,
	ID3D11Buffer* pVertexBuffer,
	ID3D11Buffer* pIndexBuffer,
	const AddressesOfMembers & addresses,
	const DirectX::XMFLOAT3 & cameraPosition,
	const DirectX::XMFLOAT3 & fogColor,
	const UINT vertexBufferStride,
	const float fogStart,
	const float fogRange,
	const bool  fogEnabled,
	const bool  useAlphaClip)
{
	// -----------------------------------------------------------------------------
	//            SETUP SHADER PARAMS WHICH ARE THE SAME FOR EACH MODEL
	// -----------------------------------------------------------------------------

	const UINT offset = 0;

	// set a ptr to the vertex buffer and vertex buffer stride
	pDeviceContext->IASetVertexBuffers(
		0,                                 // start slot
		1,                                 // num buffers
		&pVertexBuffer,                    // ppVertexBuffers
		&vertexBufferStride,               // pStrides
		&offset);                          // pOffsets

										   // set a ptr to the index buffer
	pDeviceContext->IASetIndexBuffer(
		pIndexBuffer,                      // pIndexBuffer
		DXGI_FORMAT::DXGI_FORMAT_R32_UINT, // format of the indices
		0);                                // offset, in bytes

	///////////////////////////////////

	// Set the vertex input layout
	pDeviceContext->IASetInputLayout(addresses.pVertexShaderInputLayout);

	// Set the vertex and pixels shaders that will be used to render the model
	pDeviceContext->VSSetShader(addresses.pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(addresses.pPixelShader, nullptr, 0);

	// Set the sampler state in the pixel shader
	pDeviceContext->PSSetSamplers(0, 1, addresses.ppSamplerState);

	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	// -----------------------------------------------------------------------------
	//                PIXEL SHADER: UPDATE THE CONSTANT CAMERA BUFFER
	// -----------------------------------------------------------------------------

	// prepare data for the constant camera buffer
	cameraBuffer_.data.cameraPosition = cameraPosition;

	// load camera position into GPU
	cameraBuffer_.ApplyChanges(pDeviceContext);

	// set the buffer for the vertex shader
	pDeviceContext->PSSetConstantBuffers(0, 1, addresses.cameraConstBufferAddress);

	// -----------------------------------------------------------------------------
	//                  PIXEL SHADER: UPDATE THE BUFFER PER FRAME
	// -----------------------------------------------------------------------------

	// only if fog enabled we update its params
	if (bufferPerFrame_.data.fogEnabled = fogEnabled)
	{
		bufferPerFrame_.data.fogColor = fogColor;
		bufferPerFrame_.data.fogStart = fogStart;
		bufferPerFrame_.data.fogRange = fogRange;
	}

	// do we use alpha clipping ?
	bufferPerFrame_.data.useAlphaClip = useAlphaClip;

	// load data into GPU
	bufferPerFrame_.ApplyChanges(pDeviceContext);

	// set the buffer for the vertex shader
	pDeviceContext->PSSetConstantBuffers(1, 1, addresses.bufferPerFrameAddress);

	return;
}

////////////////////////////////////////////////////////////////////
// Filename:     TerrainShaderClass.cpp
// Description:  this shader class is needed for rendering the terrain
// Created:      11.04.23
////////////////////////////////////////////////////////////////////
#include "TerrainShaderClass.h"

#include <algorithm>

TerrainShaderClass::TerrainShaderClass()
	: className_{__func__}
{
	Log::Debug(LOG_MACRO);
}

TerrainShaderClass::~TerrainShaderClass()
{
}



// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PUBLIC FUNCTIONS                                         //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// Initializes the shaders for rendering of the model
bool TerrainShaderClass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	try
	{
		const WCHAR* vsFilename = L"shaders/terrainVertex.hlsl";
		const WCHAR* psFilename = L"shaders/terrainPixel.hlsl";

		// try to initialize the vertex and pixel HLSL shaders
		InitializeShaders(pDevice, pDeviceContext, vsFilename, psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the terrain shader class");
		return false;
	}

	Log::Debug(LOG_MACRO, "is initialized");

	return true;
}

///////////////////////////////////////////////////////////

bool TerrainShaderClass::Render(ID3D11DeviceContext* pDeviceContext)
{
	try
	{
		// set the shader parameters
		SetShaderParameters(pDeviceContext);

		// render the model using this shader
		RenderShader(pDeviceContext, 10);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't render using the shader");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

const std::string & TerrainShaderClass::GetShaderName() const
{
	return className_;
}


///////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////////////////

void TerrainShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const WCHAR* vsFilename,
	const WCHAR* psFilename)
{
	// this function helps to initialize the HLSL shaders,
	// layout, sampler state, and constant buffers

	const UINT layoutElemNum = 6;                       // the number of the input layout elements
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum]; // description for the vertex input layout
	bool result = false;
	HRESULT hr = S_OK;


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

	layoutDesc[3].SemanticName = "TANGENT";
	layoutDesc[3].SemanticIndex = 0;
	layoutDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[3].InputSlot = 0;
	layoutDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[3].InstanceDataStepRate = 0;

	layoutDesc[4].SemanticName = "BINORMAL";
	layoutDesc[4].SemanticIndex = 0;
	layoutDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[4].InputSlot = 0;
	layoutDesc[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[4].InstanceDataStepRate = 0;

	layoutDesc[5].SemanticName = "COLOR";
	layoutDesc[5].SemanticIndex = 0;
	layoutDesc[5].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[5].InputSlot = 0;
	layoutDesc[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[5].InstanceDataStepRate = 0;


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

	// initialize the matrix constant buffer
	hr = this->matrixBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix buffer");

	// initialize the point light position buffer
	hr = this->pointLightPositionBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the point light position buffer");

	// initialize the light constant buffer
	hr = this->diffuseLightBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the light buffer");

	// initialize the point light color buffer
	hr = this->pointLightColorBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the point light color buffer");

	// initialize the constant camera buffer
	hr = this->cameraBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the camera buffer");

	// initialize the buffer per frame
	hr = this->bufferPerFrame_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the buffer per frame");


	return;

} // end InitializeShaders

///////////////////////////////////////////////////////////

void TerrainShaderClass::SetShaderParameters(ID3D11DeviceContext* pDeviceContext)
{
	// this function sets parameters for the HLSL shaders

#if 0

	// ---------------------------------------------------------------------------------- //
	//                 VERTEX SHADER: UPDATE THE CONSTANT MATRIX BUFFER                   //
	// ---------------------------------------------------------------------------------- //

	// prepare matrices for using in the HLSL constant matrix buffer
	matrixBuffer_.data.world         = DirectX::XMMatrixTranspose(pDataForShader->world);
	matrixBuffer_.data.view          = DirectX::XMMatrixTranspose(pDataForShader->view);
	matrixBuffer_.data.projection    = DirectX::XMMatrixTranspose(pDataForShader->projection);
	//matrixBuffer_.data.worldViewProj = DirectX::XMMatrixTranspose(pDataForShader->WVP);

	// update the matrix buffer
	result = matrixBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the matrix buffer");

	// set the buffer for the vertex shader
	pDeviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//          VERTEX SHADER: UPDATE THE CONSTANT POINT LIGHTS POSITIONS BUFFER          //
	// ---------------------------------------------------------------------------------- //

	// copy the light position variables into the constant buffer
	for (UINT i = 0; i < pointLightsArr.size(); i++)
	{
		pointLightPositionBuffer_.data.lightPosition[i] = pointLightsArr[i]->GetPosition();
	}

	// set the current number of point light sources
	pointLightPositionBuffer_.data.numPointLights = pointLightsArr.size();

	// update the point lights positions buffer
	result = pointLightPositionBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the point light position buffer");
	
	// set the buffer for the vertex shader
	pDeviceContext->VSSetConstantBuffers(1, 1, pointLightPositionBuffer_.GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//              PIXEL SHADER: UPDATE THE CONSTANT DIFFUSE LIGHTS BUFFER               //
	// ---------------------------------------------------------------------------------- //

	// write data into the buffer
	diffuseLightBuffer_.data.ambientColor   = diffuseLightsArr[0]->GetAmbientColor();
	diffuseLightBuffer_.data.diffuseColor   = diffuseLightsArr[0]->GetDiffuseColor();
	diffuseLightBuffer_.data.lightDirection = diffuseLightsArr[0]->GetDirection();

	// update the light buffer
	result = diffuseLightBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the light buffer");

	// set the constant light buffer for the HLSL pixel shader
	pDeviceContext->PSSetConstantBuffers(0, 1, diffuseLightBuffer_.GetAddressOf());

	
	// ---------------------------------------------------------------------------------- //
	//           PIXEL SHADER: UPDATE THE CONSTANT POINT LIGHTS COLORS BUFFER             //
	// ---------------------------------------------------------------------------------- //

	// copy the light colors variables into the constant buffer
	for (UINT i = 0; i < pointLightsArr.size(); i++)
	{
		pointLightColorBuffer_.data.diffuseColor[i] = pointLightsArr[i]->GetDiffuseColor();
	}

	// set the current number of point light sources
	pointLightColorBuffer_.data.numPointLights = pointLightsArr.size();

	// update the point lights colors buffer
	result = pointLightColorBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the point light color buffer");

	// set the buffer for the pixel shader
	pDeviceContext->PSSetConstantBuffers(1, 1, pointLightColorBuffer_.GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//                 PIXEL SHADER: UPDATE THE CONSTANT CAMERA BUFFER                    //
	// ---------------------------------------------------------------------------------- //

	// prepare data for the constant camera buffer
	cameraBuffer_.data.cameraPosition = pDataForShader->cameraPos;
	//cameraBuffer_.data.padding = 0.0f;

	// update the constant camera buffer
	result = cameraBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the camera buffer");

	// set the buffer for the vertex shader
	pDeviceContext->PSSetConstantBuffers(2, 1, cameraBuffer_.GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//                    PIXEL SHADER: UPDATE THE BUFFER PER FRAME                       //
	// ---------------------------------------------------------------------------------- //

	// only if fog enabled we update its params
	if (bufferPerFrame_.data.fogEnabled = pDataForShader->fogEnabled)
	{
		bufferPerFrame_.data.fogColor = pDataForShader->fogColor;
		bufferPerFrame_.data.fogStart = pDataForShader->fogStart;
		bufferPerFrame_.data.fogRange = pDataForShader->fogRange;
		bufferPerFrame_.data.fogRange_inv = pDataForShader->fogRange_inv;
	}

	// setup if we want to use normal vector values as a colour of the pixel
	bufferPerFrame_.data.debugNormals = pDataForShader->debugNormals;

	// update the constant camera buffer
	result = bufferPerFrame_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the buffer per frame");

	// set the buffer for the vertex shader
	pDeviceContext->PSSetConstantBuffers(3, 1, bufferPerFrame_.GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//                            PIXEL SHADER: SET TEXTURES                              //
	// ---------------------------------------------------------------------------------- //

	try
	{
		pDeviceContext->PSSetShaderResources(0, 1, pDataForShader->texturesMap.at("diffuse"));
		pDeviceContext->PSSetShaderResources(1, 1, pDataForShader->texturesMap.at("normals"));
	}
	// in case if there is no such a key in the textures map we catch an exception about it;
	catch (std::out_of_range & e)   
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no texture with such a key");
	}
	
#endif

	return;

} // end SetShaderParameters

///////////////////////////////////////////////////////////

void TerrainShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, 
	const UINT indexCount)
{
	// this function sets stuff which we will use for rendering with shader: 
	// layout, vertex and pixel shader, sampler state,
	// and also renders our 3D model

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
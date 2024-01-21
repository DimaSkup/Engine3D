////////////////////////////////////////////////////////////////////
// Filename:     TerrainShaderClass.cpp
// Description:  this shader class is needed for rendering the terrain
// Created:      11.04.23
////////////////////////////////////////////////////////////////////
#include "TerrainShaderClass.h"
#include <algorithm>

TerrainShaderClass::TerrainShaderClass(void)
{
	Log::Debug(THIS_FUNC_EMPTY);
	className_ = __func__;
}

// we don't use the copy constructor and destructor in this class
TerrainShaderClass::TerrainShaderClass(const TerrainShaderClass& anotherObj)
{
}

TerrainShaderClass::~TerrainShaderClass(void)
{
}



// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PUBLIC FUNCTIONS                                         //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// Initializes the shaders for rendering of the model
bool TerrainShaderClass::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd)
{
	try
	{
		const WCHAR* vsFilename = L"shaders/terrainVertex.hlsl";
		const WCHAR* psFilename = L"shaders/terrainPixel.hlsl";

		// try to initialize the vertex and pixel HLSL shaders
		InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize the terrain shader class");
		return false;
	}

	Log::Debug(THIS_FUNC, "is initialized");

	return true;
}

///////////////////////////////////////////////////////////

bool TerrainShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	DataContainerForShaders* pDataForShader)
{
	try
	{
		// set the shader parameters
		SetShaderParameters(pDeviceContext,
			pDataForShader->world,
			pDataForShader->view,
			pDataForShader->orthoOrProj,
			pDataForShader->texturesMap,       // diffuse textures / normal maps / etc.
			*(pDataForShader->ptrToDiffuseLightsArr),
			*(pDataForShader->ptrToPointLightsArr),
			pDataForShader->cameraPos);

		// render the model using this shader
		RenderShader(pDeviceContext, pDataForShader->indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't render using the shader");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

// 1. Sets the parameters for HLSL shaders which are used for rendering
// 2. Renders the model using the HLSL shaders
bool TerrainShaderClass::Render(ID3D11DeviceContext* deviceContext,
	const UINT indexCount,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,   // contains terrain textures and normal maps
	const std::vector<LightClass*>* ptrToDiffuseLightsArr,
	const std::vector<LightClass*>* ptrToPointLightsArr,
	const DirectX::XMFLOAT3 & cameraPosition)
{
	try
	{
		// set the shader parameters
		SetShaderParameters(deviceContext,
			world,
			view,
			projection,
			texturesMap,                        // diffuse textures / normal maps 
			*ptrToDiffuseLightsArr,
			*ptrToPointLightsArr,
			cameraPosition);

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


const std::string & TerrainShaderClass::GetShaderName() const _NOEXCEPT
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
	HWND hwnd,
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

	// initialize the light constant buffer
	hr = this->diffuseLightBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the light buffer");

	// initialize the point light position buffer
	hr = this->pointLightPositionBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the point light position buffer");

	// initialize the point light color buffer
	hr = this->pointLightColorBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the point light color buffer");

	// initialize the constant camera buffer
	hr = this->cameraBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the camera buffer");

	return;

} // end InitializeShaders

///////////////////////////////////////////////////////////

void TerrainShaderClass::SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,   // contains terrain textures and normal maps
	const std::vector<LightClass*> & diffuseLightsArr,
	const std::vector<LightClass*> & pointLightsArr,
	const DirectX::XMFLOAT3 & cameraPosition)
{
	// this function sets parameters for the HLSL shaders

	// the number of point lights is limited (because of the HLSL shader) so we have to
	// check the number of it
	bool result = pointLightsArr.size() <= _MAX_NUM_POINT_LIGHTS_ON_TERRAIN;
	COM_ERROR_IF_FALSE(result, "there are too many point light sources");

	// ---------------------- SET PARAMS FOR THE VERTEX SHADER -------------------------- //

	// prepare matrices for using in the HLSL constant matrix buffer
	matrixBuffer_.data.world      = DirectX::XMMatrixTranspose(world);
	matrixBuffer_.data.view       = DirectX::XMMatrixTranspose(view);
	matrixBuffer_.data.projection = DirectX::XMMatrixTranspose(projection);

	// update the matrix buffer
	result = matrixBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the matrix buffer");

	// set the buffer for the vertex shader
	pDeviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());




	// copy the light position variables into the constant buffer
	for (UINT i = 0; i < pointLightsArr.size(); i++)
	{
		pointLightPositionBuffer_.data.lightPosition[i] = pointLightsArr[i]->GetPosition();
	}

	// set the current number of point light sources
	pointLightPositionBuffer_.data.numPointLights = pointLightsArr.size();

	// update the point lights positions buffer
	result = pointLightPositionBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the point light position buffer");
	
	// set the buffer for the vertex shader
	pDeviceContext->VSSetConstantBuffers(1, 1, pointLightPositionBuffer_.GetAddressOf());

	


	// ----------------------- SET PARAMS FOR THE PIXEL SHADER -------------------------- //

	// write data into the buffer
	diffuseLightBuffer_.data.ambientColor   = diffuseLightsArr[0]->GetAmbientColor();
	diffuseLightBuffer_.data.diffuseColor   = diffuseLightsArr[0]->GetDiffuseColor();
	diffuseLightBuffer_.data.lightDirection = diffuseLightsArr[0]->GetDirection();

	// update the light buffer
	result = diffuseLightBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the light buffer");

	// set the constant light buffer for the HLSL pixel shader
	pDeviceContext->PSSetConstantBuffers(0, 1, diffuseLightBuffer_.GetAddressOf());

	

	// copy the light colors variables into the constant buffer
	for (UINT i = 0; i < pointLightsArr.size(); i++)
	{
		pointLightColorBuffer_.data.diffuseColor[i] = pointLightsArr[i]->GetDiffuseColor();
	}

	// set the current number of point light sources
	pointLightColorBuffer_.data.numPointLights = pointLightsArr.size();

	// update the point lights colors buffer
	result = pointLightColorBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the point light color buffer");

	// set the buffer for the pixel shader
	pDeviceContext->PSSetConstantBuffers(1, 1, pointLightColorBuffer_.GetAddressOf());

	// ---------------------------------------------------------------------------------- //
	//                     UPDATE THE CONSTANT CAMERA BUFFER                              //
	// ---------------------------------------------------------------------------------- //

	// prepare data for the constant camera buffer
	cameraBuffer_.data.cameraPosition = cameraPosition;
	cameraBuffer_.data.padding = 0.0f;

	// update the constant camera buffer
	result = cameraBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the camera buffer");

	// set the buffer for the vertex shader
	pDeviceContext->PSSetConstantBuffers(2, 1, cameraBuffer_.GetAddressOf());


	// --------------------- SET TEXTURES FOR THE PIXEL SHADER ------------------------- //

	try
	{
		for (UINT i = 0; i < (UINT)textureKeys_.size(); i++)
		{
			pDeviceContext->PSSetShaderResources(i, 1, texturesMap.at(textureKeys_[i]));
		}
	}
	// in case if there is no such a key in the textures map we catch an exception about it;
	catch (std::out_of_range & e)   
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "there is no texture with such a key");
	}
	
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
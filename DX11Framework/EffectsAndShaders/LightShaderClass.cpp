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
		//const WCHAR* vsFilename = L"shaders/lightVertex.hlsl";
		//const WCHAR* psFilename = L"shaders/lightPixel.hlsl";
		WCHAR* fxFilename = L"shaders/DiffuseLight.fx";

		InitializeShaders(pDevice, pDeviceContext, fxFilename);
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

		ID3DX11EffectMatrixVariable* pfxWorld = pfxWorld_;
		ID3DX11EffectMatrixVariable* pfxWVP = pfxWorldViewProj_;   // ptr to the effect variable of const matrix which contains WORLD*VIEW*PROJ matrix

		ID3DX11EffectTechnique* pTech = pFX_->GetTechniqueByName("DiffuseLightTech");
		D3DX11_TECHNIQUE_DESC techDesc;

		pTech->GetDesc(&techDesc);




		// ---------------------------------------------------------------------------------- //
		//               SETUP SHADER PARAMS WHICH ARE THE SAME FOR EACH MODEL                //
		// ---------------------------------------------------------------------------------- //

		// set stuff which we will use: layout, vertex and pixel shader,
		// sampler state

		// set the input layout for the vertex shader
		pDeviceContext->IASetInputLayout(pInputLayout_);

		// set the sampler state for the pixel shader
		pDeviceContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

		// set a ptr to the vertex buffer and vertex buffer stride
		pDeviceContext->IASetVertexBuffers(0, 1,
			&pVertexBuffer,
			&vertexBufferStride,
			&offset);

		// set a ptr to the index buffer
		pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);


		// setup ambient and diffuse light sources for shader
		pfxAmbientColor_->SetFloatVector((float*)&diffuseLights.ambientColors_[0]);
		pfxAmbientLightStrength_->SetFloat(1.0f);

		pfxDiffuseLightColor_->SetFloatVector((float*)&(diffuseLights.diffuseColors_[0]));
		pfxDiffuseLightStrength_->SetFloat(diffuseLights.diffusePowers_[0]);
		pfxDiffuseLightDirection_->SetFloatVector((float*)&(diffuseLights.directions_[0]));


		// setup camera position for shader
		pfxCameraPos_->SetFloatVector((float*)&(cameraPosition));


		pfxIsFogEnabled_->SetFloat(1.0f);
		//pfxIsDebugNormals_->SetFloat(0.0f);

		pfxFogStart_->SetFloat(fogStart);
		pfxFogRange_->SetFloat(fogRange);
		pfxFogColor_->SetFloatVector((float*)&(fogColor));

		// ------------------------------------------------------------------------------ //
		//    SETUP SHADER PARAMS WHICH ARE DIFFERENT FOR EACH MODEL AND RENDER MODELS    //
		// ------------------------------------------------------------------------------ //

		// go through each model, prepare it for rendering using the shader, and render it
		for (UINT idx = 0; idx < worldMatrices.size(); ++idx)
		{
			// ------------ VERTEX SHADER: UPDATE THE CONSTANT MATRIX BUFFER ------------ //
			pfxWorld->SetMatrix((float*)&(worldMatrices[idx]));
			pfxWVP->SetMatrix((float*)&(worldMatrices[idx] * viewProj));  // set (world * view_proj)

			pDeviceContext->PSSetShaderResources(0, 1, ppDiffuseTextures[idx]);

			for (UINT pass = 0; pass < techDesc.Passes; ++pass)
			{
				pTech->GetPassByIndex(pass)->Apply(0, pDeviceContext);

				// draw geometry
				pDeviceContext->DrawIndexed(indexCount, 0, 0);
			}
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
	WCHAR* fxFilename)
{
	bool result = false;
	HRESULT hr = S_OK;
	const UINT layoutElemNum = 3;                       // the number of the input layout elements
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum]; // description for the vertex input layout
	D3DX11_PASS_DESC passDesc;
	ID3DX11Effect* pFX = nullptr;

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


	// ---------------------------------- EFFECTS --------------------------------------- //

	// compile and create the color FX effect
	hr = ShaderClass::CompileAndCreateEffect(fxFilename, &pFX, pDevice);
	COM_ERROR_IF_FAILED(hr, "can't compile/create an effect");

	// setup the effect variables
	pfxWorld_ = pFX->GetVariableByName("gWorldMatrix")->AsMatrix();
	pfxWorldViewProj_ = pFX->GetVariableByName("gWorldViewProj")->AsMatrix();


	// variables for the pixel shader
	pfxAmbientColor_ = pFX->GetVariableByName("gAmbientColor")->AsVector();
	pfxAmbientLightStrength_ = pFX->GetVariableByName("gAmbientLightStrength")->AsScalar();
	pfxDiffuseLightColor_ = pFX->GetVariableByName("gDiffuseLightColor")->AsVector();
	pfxDiffuseLightStrength_ = pFX->GetVariableByName("gDiffuseLightStrength")->AsScalar();
	pfxDiffuseLightDirection_ = pFX->GetVariableByName("gDiffuseLightDirection")->AsVector();

	pfxCameraPos_ = pFX->GetVariableByName("gCameraPos")->AsVector();
	pfxIsFogEnabled_ = pFX->GetVariableByName("gFogEnabled")->AsScalar();
	pfxIsDebugNormals_ = pFX->GetVariableByName("gDebugNormals")->AsScalar();

	pfxFogStart_ = pFX->GetVariableByName("gFogStart")->AsScalar();
	pfxFogRange_ = pFX->GetVariableByName("gFogRange")->AsScalar();
	pfxFogColor_ = pFX->GetVariableByName("gFogColor")->AsVector();

	// setup the pointer to the effect technique
	pTech_ = pFX->GetTechniqueByName("DiffuseLightTech");

	// store a pointer to the effect
	pFX_ = pFX;

	// create the input layout using the fx technique
	
	pTech_->GetPassByIndex(0)->GetDesc(&passDesc);

	hr = pDevice->CreateInputLayout(
		layoutDesc,
		layoutElemNum,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&pInputLayout_);
	COM_ERROR_IF_FAILED(hr, "can't create the input layout");


	// ------------------------------- SAMPLER STATE ------------------------------------ //

	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the sampler state");

	return;
}

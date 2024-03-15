/////////////////////////////////////////////////////////////////////
// Filename: colorshaderclass.cpp
// Revising: 06.04.22
/////////////////////////////////////////////////////////////////////
#include "colorshaderclass.h"


ColorShaderClass::ColorShaderClass()
	: className_ { __func__ }
{
	Log::Debug(LOG_MACRO);
}

ColorShaderClass::~ColorShaderClass()
{
	_RELEASE(pInputLayout_);
	_RELEASE(pFX_);
}


// ------------------------------------------------------------------------------ //
//
//                         PUBLIC FUNCTIONS
//
// ------------------------------------------------------------------------------ //
bool ColorShaderClass::Initialize(ID3D11Device* pDevice, 
	                              ID3D11DeviceContext* pDeviceContext)
{
	// THIS FUNCTION initializes the ColorShaderClass

	try
	{
		//const WCHAR* vsFilename = L"shaders/colorVertex.hlsl";
		//const WCHAR* psFilename = L"shaders/colorPixel.hlsl";

		WCHAR* fxFilename = L"shaders/Color.fx";
	

		InitializeShaders(pDevice, pDeviceContext, fxFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the color shader class");
		return false;
	}

	Log::Debug(LOG_MACRO, "is initialized");

	return true;
}

///////////////////////////////////////////////////////////

bool ColorShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	ID3D11Buffer* vertexBufferPtr,
	ID3D11Buffer* indexBufferPtr,
	const UINT vertexBufferStride,
	const UINT indexCount,
	const std::vector<DirectX::XMMATRIX> & worldMatrices,
	const DirectX::XMMATRIX & viewProj,
	const DirectX::XMVECTOR & color)
{
	try
	{
		bool result = false;
		const UINT offset = 0;

		// setup the pointer to the effect technique according to the input color
		if (color.m128_f32[3] == 0.0f)
			pTech_ = pFX_->GetTechniqueByName("VertexColorTech");
		else
		{
			pTech_ = pFX_->GetTechniqueByName("ConstantColorTech");
			pfxColor_->SetFloatVector((float*)&color);
		}

		// -------------------------------------------------------------------------- //
		//         SETUP SHADER PARAMS WHICH ARE THE SAME FOR EACH MODEL              //
		// -------------------------------------------------------------------------- //

		// set the input layout for the vertex shader
		pDeviceContext->IASetInputLayout(pInputLayout_);

		// set a ptr to the vertex buffer and vertex buffer stride
		pDeviceContext->IASetVertexBuffers(0, 1,
			&vertexBufferPtr,
			&vertexBufferStride,
			&offset);

		// set a ptr to the index buffer
		pDeviceContext->IASetIndexBuffer(indexBufferPtr, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);



		// -------------------------------------------------------------------------- //
		//                        PIXEL SHADER: COLOR                                 //
		// -------------------------------------------------------------------------- //



	
		// ------------------------------------------------------------------------------ //
		//    SETUP SHADER PARAMS WHICH ARE DIFFERENT FOR EACH MODEL AND RENDER MODELS    //
		// ------------------------------------------------------------------------------ //

		D3DX11_TECHNIQUE_DESC techDesc;
		pTech_->GetDesc(&techDesc);

		// go through each model, prepare it for rendering using the shader, and render it
		for (UINT idx = 0; idx < worldMatrices.size(); ++idx)
		{

			// ------------ VERTEX SHADER: UPDATE THE CONSTANT MATRIX BUFFER ------------ //

			// set (world * view_proj)
			pfxWorldViewProj_->SetMatrix((float*)&(worldMatrices[idx] * viewProj));

			for (UINT pass = 0; pass < techDesc.Passes; ++pass)
			{
				pTech_->GetPassByIndex(pass)->Apply(0, pDeviceContext);

				// draw geometry
				pDeviceContext->DrawIndexed(indexCount, 0, 0);
			}
		} 
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't render geometry");
	}

	return true;
}

///////////////////////////////////////////////////////////

const std::string & ColorShaderClass::GetShaderName() const
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
	WCHAR* fxFilename)
	//const WCHAR* vsFilename,
	//const WCHAR* psFilename)
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
	const UINT colorOffset = (4 * sizeof(DirectX::XMFLOAT3)) + sizeof(DirectX::XMFLOAT2);


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

	// ---------------------------------- EFFECTS --------------------------------------- //

	// compile and create the color FX effect
	hr = ShaderClass::CompileAndCreateEffect(fxFilename, &pFX_, pDevice);
	COM_ERROR_IF_FAILED(hr, "can't compile/create an effect");

	// setup the pointer to the effect technique
	pTech_ = pFX_->GetTechniqueByName("ConstantColorTech");

	// setup the effect variables
	pfxWorldViewProj_ = pFX_->GetVariableByName("gWorldViewProj")->AsMatrix();
	pfxColor_ = pFX_->GetVariableByName("gColor")->AsVector();

	// create the input layout using the fx technique
	D3DX11_PASS_DESC passDesc;
	pTech_->GetPassByIndex(0)->GetDesc(&passDesc);

	hr = pDevice->CreateInputLayout(
		layoutDesc,
		layoutElemNum,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&pInputLayout_);
	COM_ERROR_IF_FAILED(hr, "can't create the input layout");

	return;
}

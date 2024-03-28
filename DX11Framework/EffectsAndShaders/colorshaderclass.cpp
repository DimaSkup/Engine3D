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

void ColorShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	ID3D11Buffer* vertexBufferPtr,
	ID3D11Buffer* indexBufferPtr,
	const UINT vertexBufferStride,
	const UINT indexCount,
	const std::vector<DirectX::XMMATRIX> & worldMatrices,
	const DirectX::XMMATRIX & viewProj,
	const float totalGameTime,            // time passed since the start of the application
	const DirectX::XMVECTOR & color)      // default: {0,0,0,0} (zero-vector)
{
	// THIS FUNCTION renders the input vertex buffer using the Color effect
	// and for painting vertices with some color we use its colors

	try
	{
		const UINT offset = 0;
		ID3DX11EffectTechnique* pTech = nullptr;
		ID3DX11EffectMatrixVariable* pfxWVP = pfxWorldViewProj_;   // ptr to the effect variable of const matrix which contains WORLD*VIEW*PROJ matrix
		//ID3DX11EffectVectorVariable* pfxColor = pfxColor_;
		//ID3DX11EffectScalarVariable* pfxTotalGameTime_ = nullptr;
		D3DX11_TECHNIQUE_DESC techDesc;

	
		
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

		// setup the pointer to the effect technique and get description of this tech
		if (DirectX::XMVectorGetW(color) == 0.0f)
		{
			pTech = pFX_->GetTechniqueByName("VertexColorTech");
			pTech->GetDesc(&techDesc);
			pfxTotalGameTime_->SetFloat(totalGameTime);
		}
		else
		{
			// setup the pointer to the effect technique and get description of this tech
			pTech = pFX_->GetTechniqueByName("ConstantColorTech");;
			pTech->GetDesc(&techDesc);
			pfxTotalGameTime_->SetFloat(totalGameTime);
			pfxColor_->SetFloatVector((float*)&(color));                   // set color for all the model which will be rendered
		}


		// ------------------------------------------------------------------------------ //
		//    SETUP SHADER PARAMS WHICH ARE DIFFERENT FOR EACH MODEL AND RENDER MODELS    //
		// ------------------------------------------------------------------------------ //

		// go through each model, prepare it for rendering using the shader, and render it
		for (UINT idx = 0; idx < worldMatrices.size(); ++idx)
		{

			// ------------ VERTEX SHADER: UPDATE THE CONSTANT MATRIX BUFFER ------------ //

			pfxWVP->SetMatrix((float*)&(worldMatrices[idx] * viewProj));  // set (world * view_proj)

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
		Log::Error(LOG_MACRO, "can't render geometry");
	}

	return;
}

///////////////////////////////////////////////////////////

void ColorShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	ID3D11Buffer* vertexBufferPtr,
	ID3D11Buffer* indexBufferPtr,
	const UINT vertexBufferStride,
	const UINT indexCount,
	const std::vector<DirectX::XMMATRIX> & worldMatrices,
	const DirectX::XMMATRIX & viewProj,
	const float totalGameTime,            // time passed since the start of the application
	const std::vector<DirectX::XMFLOAT4> & colorsArr)
{
	// THIS FUNCTION renders each model with its unique color;
	// so sizes of the worldMatrices array and the colorsArr must be the same

	try
	{
		assert(worldMatrices.size() == colorsArr.size());

		const UINT offset = 0;
		ID3DX11EffectTechnique* pTech = nullptr;
		ID3DX11EffectMatrixVariable* pfxWVP = pfxWorldViewProj_;   // ptr to the effect variable of const matrix which contains WORLD*VIEW*PROJ matrix
		ID3DX11EffectVectorVariable* pfxColor = pfxColor_;
		D3DX11_TECHNIQUE_DESC techDesc;

		pfxTotalGameTime_->SetFloat(totalGameTime);


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

		// setup the pointer to the effect technique and get description of this tech
		pTech = pFX_->GetTechniqueByName("ConstantColorTech");;
		pTech->GetDesc(&techDesc);


		// ------------------------------------------------------------------------------ //
		//    SETUP SHADER PARAMS WHICH ARE DIFFERENT FOR EACH MODEL AND RENDER MODELS    //
		// ------------------------------------------------------------------------------ //




		// go through each model, prepare it for rendering using the shader, and render it
		for (UINT idx = 0; idx < worldMatrices.size(); ++idx)
		{

			// ------------ VERTEX SHADER: UPDATE THE CONSTANT MATRIX BUFFER ------------ //

			pfxWVP->SetMatrix((float*)&(worldMatrices[idx] * viewProj));  // set (world * view_proj)
			pfxColor->SetFloatVector((float*)&(colorsArr[idx]));          // set different color for each model


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
		Log::Error(LOG_MACRO, "can't render geometry");
	}


	return;
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
	//layoutDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layoutDesc[1].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	layoutDesc[1].InputSlot = 0;
	layoutDesc[1].AlignedByteOffset = colorOffset;  
	layoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[1].InstanceDataStepRate = 0;


	// ---------------------------------- EFFECTS --------------------------------------- //

	// compile and create the color FX effect
	ID3DX11Effect* pFX = nullptr;
	hr = ShaderClass::CompileAndCreateEffect(fxFilename, &pFX, pDevice);
	COM_ERROR_IF_FAILED(hr, "can't compile/create an effect");

	// setup the pointer to the effect technique
	pTech_ = pFX->GetTechniqueByName("ConstantColorTech");

	// setup the effect variables
	pfxWorldViewProj_ = pFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	pfxColor_         = pFX->GetVariableByName("gColor")->AsVector();
	pfxTotalGameTime_ = pFX->GetVariableByName("gTime")->AsScalar();

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

	// setup the member ptr to the effect
	pFX_ = pFX;

	return;
}

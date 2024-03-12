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
	const DirectX::XMFLOAT4 & color)
{
	try
	{
		bool result = false;
		const UINT offset = 0;

		// -------------------------------------------------------------------------- //
		//         SETUP SHADER PARAMS WHICH ARE THE SAME FOR EACH MODEL              //
		// -------------------------------------------------------------------------- //

		// set the input layout for the vertex shader
		//pDeviceContext->IASetInputLayout(vertexShader_.GetInputLayout());
#if 0
		// set shader which we will use for rendering
		pDeviceContext->VSSetShader(vertexShader_.GetShader(), nullptr, 0);
		pDeviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0);
#endif
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
#if 0
		// update the color buffer;
		colorBuffer_.data.rgbaColor = color;

		result = colorBuffer_.ApplyChanges(pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't update the alpha color buffer");

		// set the constant buffer for the vertex shader
		pDeviceContext->VSSetConstantBuffers(1, 1, colorBuffer_.GetAddressOf());
#endif

		D3DX11_TECHNIQUE_DESC techDesc;
		ID3DX11EffectTechnique* mTech;
		mTech = fx_->GetTechniqueByName("ColorTech");
		mTech->GetDesc(&techDesc);

		ID3DX11EffectMatrixVariable* fxWVPVar;
		fxWVPVar = fx_->GetVariableByName("gWorldViewProj")->AsMatrix();
		
		// ------------------------------------------------------------------------------ //
		//    SETUP SHADER PARAMS WHICH ARE DIFFERENT FOR EACH MODEL AND RENDER MODELS    //
		// ------------------------------------------------------------------------------ //

		// go through each model, prepare it for rendering using the shader, and render it
		for (UINT idx = 0; idx < worldMatrices.size(); ++idx)
		{

			// ------------ VERTEX SHADER: UPDATE THE CONSTANT MATRIX BUFFER ------------ //

			// set (world * view_proj)
			fxWVPVar->SetMatrix((float*)&(DirectX::XMMatrixTranspose(worldMatrices[idx] * viewProj)));

			for (UINT pass = 0; pass < techDesc.Passes; ++pass)
			{
				mTech->GetPassByIndex(pass)->Apply(0, pDeviceContext);

				// draw some geometry
			}
#if 0
			// update the matrix const buffer
			matrixBuffer_.data.world = DirectX::XMMatrixTranspose(worldMatrices[idx]);
			matrixBuffer_.data.worldViewProj = DirectX::XMMatrixTranspose(worldMatrices[idx] * viewProj);

			result = matrixBuffer_.ApplyChanges(pDeviceContext);
			COM_ERROR_IF_FALSE(result, "can't update the matrix const buffer");

			// set the constant buffer for the vertex shader
			pDeviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());
#endif
			// render the model
			pDeviceContext->DrawIndexed(indexCount, 0, 0);

		} // for
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't render the model");
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

	layoutDesc[1].SemanticName = "COLOR";
	layoutDesc[1].SemanticIndex = 0;
	layoutDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layoutDesc[1].InputSlot = 0;
	layoutDesc[1].AlignedByteOffset = colorOffset;  
	layoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[1].InstanceDataStepRate = 0;

	// ---------------------------------- EFFECTS --------------------------------------- //

	// compile and create the color effect
	hr = ShaderClass::CompileAndCreateEffect(fxFilename, fx_, pDevice);
	COM_ERROR_IF_FAILED(hr, "can't compile/create an effect");


	// ---------------------------------- SHADERS --------------------------------------- //

	// initialize the vertex shader
	//result = this->vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	//COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	//result = this->pixelShader_.Initialize(pDevice, psFilename);
	//COM_ERROR_IF_FALSE(result, "can't initialize the pixel shader");


	// ------------------------------- CONSTANT BUFFERS --------------------------------- //

	// initialize the matrix const buffer
	//hr = matrixBuffer_.Initialize(pDevice, pDeviceContext);
	//COM_ERROR_IF_FAILED(hr, "can't initialize the matrix buffer");

	// initialize the color const buffer
	//hr = colorBuffer_.Initialize(pDevice, pDeviceContext);
	//COM_ERROR_IF_FAILED(hr, "can't initialize the color buffer");


	return;
}

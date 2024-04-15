// *********************************************************************************
// Filename:    Vertex.cpp
// Description: contains constructors for the VERTEX type;
//              implements some functional for the InputLayouts class;
// 
// Created:     13.04.24
// *********************************************************************************

#include "Vertex.h"

#pragma region VERTEX_Constructors
// default constructor
VERTEX::VERTEX() :
	position(0.0f, 0.0f, 0.0f),
	texture(0.0f, 0.0f),
	normal(0.0f, 0.0f, 0.0f),
	tangent(0.0f, 0.0f, 0.0f),
	binormal(0.0f, 0.0f, 0.0f),
	color(1.0f, 1.0f, 1.0f, 1.0f)  // a default color of a vertex is pink
{
}

// a constructor with raw input params
VERTEX::VERTEX(const float posX, const float posY, const float posZ,
	const float texX, const float texY,
	const float normalX, const float normalY, const float normalZ,
	const float tangentX, const float tangentY, const float tangentZ,
	const float binormalX, const float binormalY, const float binormalZ,
	const float red, const float green, const float blue, const float alpha)  // default color { 1, 1, 1, 1 }
	: position { posX, posY, posZ }
	, texture { texX, texY }
	, normal { normalX, normalY, normalZ }
	, tangent { tangentX, tangentY, tangentZ }
	, binormal { binormalX, binormalY, binormalZ }
	, color { red, green, blue, alpha }
{
}

// a constructor with XM-type input params
VERTEX::VERTEX(const DirectX::XMFLOAT3 & pos,
		const DirectX::XMFLOAT2 & tex,
		const DirectX::XMFLOAT3 & nor,
		const DirectX::XMFLOAT3 & tang,
		const DirectX::XMFLOAT3 & binorm,
		const DirectX::PackedVector::XMCOLOR & col)
	:
	position(pos),
	texture(tex),
	normal(nor),
	tangent(tang),
	binormal(binorm),
	color(col)
{
}

#pragma endregion


// *********************************************************************************
//                               INPUT LAYOUTS
// *********************************************************************************

#pragma region InputLayoutsDesc

D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::posNormal[2] = {{}, {}};

void InputLayoutDesc::InitDesc()
{
	D3D11_INPUT_ELEMENT_DESC layoutDesc[2];

	// set the description for the input layout
	layoutDesc[0].SemanticName = "POSITION";
	layoutDesc[0].SemanticIndex = 0;
	layoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[0].InputSlot = 0;
	layoutDesc[0].AlignedByteOffset = 0;
	layoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[0].InstanceDataStepRate = 0;


	layoutDesc[1].SemanticName = "NORMAL";
	layoutDesc[1].SemanticIndex = 0;
	layoutDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[1].InputSlot = 0;
	layoutDesc[1].AlignedByteOffset = sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT2);  // sizeof(position) + sizeof(texture_coords)
	layoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[1].InstanceDataStepRate = 0;


	// initialize the input layout description variable
	InputLayoutDesc::posNormal[0] = layoutDesc[0];
	InputLayoutDesc::posNormal[1] = layoutDesc[1];
}

#pragma endregion

#pragma region InputLayouts

// initialize static pointers to the input layout objects with nullptr
ID3D11InputLayout* InputLayouts::pPosNormal_InputLayout_ = nullptr;


void InputLayouts::InitAll(ID3D11Device* pDevice)
{
	//
	// pPosNormal_InputLayout_
	//
#if 0
	D3DX11_PASS_DESC passDesc;

	// create the input layout using the fx technique
	Effects::pBasicFX_->pLight1Tech_->GetPassByIndex(0)->GetDesc(&passDesc);

	const HRESULT hr = pDevice->CreateInputLayout(
		InputLayoutDesc::posNormal,
		2,                              // we have two elements in the input layout
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&pPosNormal_InputLayout_);
	COM_ERROR_IF_FAILED(hr, "can't create the input layout");
#endif
}

///////////////////////////////////////////////////////////

void InputLayouts::DestroyAll()
{
	//_RELEASE(pPosNormal_InputLayout_);
}

#pragma endregion

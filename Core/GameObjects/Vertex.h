// *********************************************************************************
// Filename:    Vertex.h
// Description: contains vertex structures are used in the engine;
//              also defines the input layout descriptions, and creates a single 
//              instance of each input layout and expose it globally by storing it 
//              as public static variable in an InputLayouts class;
// 
// *********************************************************************************

#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>



struct VERTEX_FONT
{
	//
	// a structure type for rendering of 2D font
    // (the VERTEX_FONT structure must match both in the FontClass and TextStore)
    //

	VERTEX_FONT() : position(0.0f, 0.0f, 0.0f), texture(0.0f, 0.0f) {}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture;
};

// *********************************************************************************

struct VERTEX
{
	//
	// a VERTEX structure type for 3D vertices
	//

	// default constructor
	VERTEX();

	// a constructor with raw input params
	VERTEX(const float posX, const float posY, const float posZ,
		const float texX, const float texY,
		const float normalX, const float normalY, const float normalZ,
		const float tangentX, const float tangentY, const float tangentZ,
		const float binormalX, const float binormalY, const float binormalZ,
		const float red = 1.0f, const float green = 1.0f, const float blue = 1.0f, const float alpha = 1.0f);

	// a constructor with XM-type input params
	VERTEX(const DirectX::XMFLOAT3 & pos,
		const DirectX::XMFLOAT2 & tex,
		const DirectX::XMFLOAT3 & nor,
		const DirectX::XMFLOAT3 & tang,
		const DirectX::XMFLOAT3 & binorm,
		const DirectX::PackedVector::XMCOLOR & col);

	// ---------------------------------------------------- //

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture;
	DirectX::XMFLOAT3 normal;
	
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 binormal;
	DirectX::PackedVector::XMCOLOR color;   // 32-bit ARGB packed color
	//DirectX::XMFLOAT4 color;
};



// *********************************************************************************
//                            INPUT LAYOUTS
// *********************************************************************************

class InputLayoutDesc
{
public:
	void InitDesc();

	static D3D11_INPUT_ELEMENT_DESC posNormal[2];
};

class InputLayouts
{
public:
	static void InitAll(ID3D11Device* pDevice);
	static void DestroyAll();

	static ID3D11InputLayout* pPosNormal_InputLayout_;
};

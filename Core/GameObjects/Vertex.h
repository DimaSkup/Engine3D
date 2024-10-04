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


using namespace DirectX;

struct VertexFont
{
	//
	// a structure type for rendering of 2D font
    // (the VertexFont structure must match both in the FontClass and TextStore)
    //

	VertexFont() : position(0.0f, 0.0f), texture(0.0f, 0.0f) {}

	XMFLOAT2 position;
	XMFLOAT2 texture;
};


// *********************************************************************************

class Vertex3D
{
	//
	// a Vertex3D structure type for 3D vertices
	//

public:
	// default constructor
	Vertex3D();

	Vertex3D(Vertex3D&& rhs);

	Vertex3D(const Vertex3D& rhs);
	Vertex3D& operator=(const Vertex3D& rhs);


	// a constructor with raw input params
	Vertex3D(
		const float posX,          const float posY,          const float posZ,
		const float texX = 0,      const float texY = 0,
		const float normalX = 0,   const float normalY = 0,   const float normalZ = 0,
		const float tangentX = 0,  const float tangentY = 0,  const float tangentZ = 0,
		const float binormalX = 0, const float binormalY = 0, const float binormalZ = 0,
		const float red = 1,       const float green = 1,     const float blue = 1, const float alpha = 1);


	// a constructor with XM-type input params
	Vertex3D(
		const DirectX::XMFLOAT3& pos,
		const DirectX::XMFLOAT2& tex,
		const DirectX::XMFLOAT3& nor,
		const DirectX::XMFLOAT3& tang,
		const DirectX::XMFLOAT3& binorm ,
		const DirectX::PackedVector::XMCOLOR& col);    // ARGB

	Vertex3D(
		DirectX::XMFLOAT3&& pos,
		DirectX::XMFLOAT2&& tex,
		DirectX::XMFLOAT3&& nor,
		DirectX::XMFLOAT3&& tang,
		DirectX::XMFLOAT3&& binorm,
		DirectX::PackedVector::XMCOLOR&& col);   // ARGB

	// ---------------------------------------------------- //

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture;
	DirectX::XMFLOAT3 normal;
	
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 binormal;
	DirectX::PackedVector::XMCOLOR color = { 1.0f, 1.0f, 0.0f, 1.0f };   // 32-bit ARGB packed color
	//DirectX::XMFLOAT4 color;
};
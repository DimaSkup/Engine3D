// *********************************************************************************
// Filename:    Vertex.cpp
// Description: contains constructors for the Vertex3D type;
//              implements some functional for the InputLayouts class;
// 
// Created:     13.04.24
// *********************************************************************************

#include "Vertex.h"

#pragma region VERTEX_Constructors


Vertex3D::Vertex3D() :
	position(0.0f, 0.0f, 0.0f),
	texture(0.0f, 0.0f),
	normal(0.0f, 0.0f, 0.0f),
	tangent(0.0f, 0.0f, 0.0f),
	binormal(0.0f, 0.0f, 0.0f),
	color(1.0f, 1.0f, 1.0f, 1.0f)  // a default color of a vertex is pink
{
	// default constructor
}

///////////////////////////////////////////////////////////

Vertex3D::Vertex3D(const Vertex3D& rhs)
{
	*this = rhs;
}

///////////////////////////////////////////////////////////

Vertex3D::Vertex3D(Vertex3D&& rhs)
{
	position = rhs.position;
	texture  = rhs.texture;
	normal   = rhs.normal;
	tangent  = rhs.tangent;
	binormal = rhs.binormal;
	color    = rhs.color;
}

///////////////////////////////////////////////////////////

Vertex3D& Vertex3D::operator=(const Vertex3D& rhs)
{
	position = rhs.position;
	texture  = rhs.texture;
	normal   = rhs.normal;
	tangent  = rhs.tangent;
	binormal = rhs.binormal;
	color    = rhs.color;

	return *this;
}

///////////////////////////////////////////////////////////

Vertex3D::Vertex3D(
	const float posX, const float posY, const float posZ,
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
	// a constructor with raw input params
}

///////////////////////////////////////////////////////////

Vertex3D::Vertex3D(
	const DirectX::XMFLOAT3 & pos,
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
	// a constructor with XM-type input params
}

Vertex3D::Vertex3D(
	DirectX::XMFLOAT3&& pos,
	DirectX::XMFLOAT2&& tex,
	DirectX::XMFLOAT3&& nor,
	DirectX::XMFLOAT3&& tang,
	DirectX::XMFLOAT3&& binorm,
	DirectX::PackedVector::XMCOLOR&& col)   // ARGB
	:
	position(pos),
	texture(tex),
	normal(nor),
	tangent(tang),
	binormal(binorm),
	color(col)
{
	// a constructor with XM-type input params
}

#pragma endregion
#pragma once
#include <DirectXMath.h>
#include <DirectXPackedVector.h>


// The VERTEX_FONT structure must match both in the FontClass and TextStore
struct VERTEX_FONT
{
	VERTEX_FONT() : position(0.0f, 0.0f, 0.0f), texture(0.0f, 0.0f) {}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture;
};

struct VERTEX
{
	// default constructor
	VERTEX():
		position(0.0f, 0.0f, 0.0f),
		texture(0.0f, 0.0f),
		normal(0.0f, 0.0f, 0.0f),
		tangent(0.0f, 0.0f, 0.0f),
		binormal(0.0f, 0.0f, 0.0f),
		color(1.0f, 1.0f, 1.0f, 1.0f)  // a default color of a vertex is pink
	{}

	// a constructor with raw input params
	VERTEX(float posX, float posY, float posZ, 
		float texX, float texY, 
		float normalX, float normalY, float normalZ,
		float tangentX, float tangentY, float tangentZ,
		float binormalX, float binormalY, float binormalZ,
		float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f)
	{
		position = { posX, posY, posZ };
		texture = { texX, texY };
		normal = { normalX, normalY, normalZ };
		tangent = { tangentX, tangentY, tangentZ };
		binormal = { binormalX, binormalY, binormalZ };
		color = { red, green, blue, alpha };
	}

	// a constructor with XM-type input params
	VERTEX(DirectX::XMFLOAT3 pos,
		DirectX::XMFLOAT2 tex,
		DirectX::XMFLOAT3 nor,
		DirectX::XMFLOAT3 tang,
		DirectX::XMFLOAT3 binorm,
		//DirectX::XMFLOAT4 col)
		const DirectX::PackedVector::XMCOLOR & col)
		: 
		position(pos), 
		texture(tex), 
		normal(nor), 
		tangent(tang),
		binormal(binorm),
		color(col)
	{}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture;
	DirectX::XMFLOAT3 normal;
	
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 binormal;
	DirectX::PackedVector::XMCOLOR color;   // 32-bit ARGB packed color
	//DirectX::XMFLOAT4 color;
};
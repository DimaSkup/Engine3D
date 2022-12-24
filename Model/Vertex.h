#pragma once
#include <DirectXMath.h>


// The VERTEX_FONT structure must match both in the FontClass and TextClass
struct VERTEX_FONT
{
	VERTEX_FONT() : position(0.0f, 0.0f, 0.0f), texture(0.0f, 0.0f) {}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture;
};

struct VERTEX
{
	VERTEX() {}
	VERTEX(float posX, float posY, float posZ, 
		float texX, float texY, 
		float normalX, float normalY, float normalZ,
		float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f)
	{
		position = { posX, posY, posZ };
		texture = { texX, texY };
		normal = { normalX, normalY, normalZ };
		color = { red, green, blue, alpha };
	}

	VERTEX(DirectX::XMFLOAT3 pos,
		DirectX::XMFLOAT2 tex,
		DirectX::XMFLOAT3 nor,
		DirectX::XMFLOAT4 col)
		: position(pos), texture(tex), normal(nor), color(col) {}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 color;
};
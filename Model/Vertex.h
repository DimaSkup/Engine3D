#pragma once
#include <DirectXMath.h>

struct VERTEX
{
	VERTEX() {}
	VERTEX(float posX, float posY, float posZ, 
		float texX, float texY, 
		float normalX, float normalY, float normalZ,
		float red = 1.0f, float green = 1.0f, float blue = 1.0f)
	{
		position = { posX, posY, posZ };
		texture = { texX, texY };
		normal = { normalX, normalY, normalZ };
		color = { red, green, blue };
	}

	VERTEX(DirectX::XMFLOAT3 pos,
		DirectX::XMFLOAT2 tex,
		DirectX::XMFLOAT3 nor,
		DirectX::XMFLOAT3 col)
		: position(pos), texture(tex), normal(nor), color(col) {}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 color;
};
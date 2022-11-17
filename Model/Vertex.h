#pragma once
#include <DirectXMath.h>

struct VERTEX_2D
{
	VERTEX_2D() {}
	VERTEX_2D(float x, float y, float z, 
		      float r, float g, float b, float a = 1.0f)
		: position(x, y, z), color(r, g, b, a) {}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
};

struct VERTEX_3D
{
	VERTEX_3D() {}
	VERTEX_3D(float posX, float posY, float posZ, 
		float texX, float texY, 
		float normalX, float normalY, float normalZ)
	{
		position = { posX, posY, posZ };
		texture = { texX, texY };
		normal = { normalX, normalY, normalZ };
	}

	VERTEX_3D(DirectX::XMFLOAT3 pos,
		DirectX::XMFLOAT2 tex,
		DirectX::XMFLOAT3 nor)
	{
		position = pos;
		texture = tex;
		normal = nor;
	}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture;
	DirectX::XMFLOAT3 normal;
};
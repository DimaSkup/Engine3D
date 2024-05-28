// ********************************************************************************
// Filename:     InitGraphicsHelperDataTypes.h
// Description:  contains different types which are used 
//               during graphics initialization
// 
// Created:      28.05.24
// ********************************************************************************
#pragma once

#include <DirectXMath.h>
#include <map>
#include <vector>
#include <assimp/material.h>
#include "../Light/LightHelper.h"
#include "../GameObjects/MeshHelperTypes.h"


struct RGBA_COLOR
{
	RGBA_COLOR(
		const float red,
		const float green, 
		const float blue, 
		const float alpha, 
		bool rangeFrom0to255 = false)
	{
		// values are in a range [0, 255]
		if (rangeFrom0to255)  
		{
			const float inv_255 = 1.0f / 255.0f;
			r = red * inv_255;
			g = green * inv_255;
			b = blue * inv_255;
			a = alpha;
		}

		// values are in a range [0, 1] 
		else 
		{
			r = red;
			g = green;
			b = blue;
			a = alpha;
		}
	}

	union
	{
		float rgba[4];
		float r, g, b, a;
	};
};

struct DataForMeshInit
{
	std::string meshID;
	
	std::map<aiTextureType, TextureClass*> texturesMap;   // 'texture_type' => 'ptr_to_texture_obj'
	Material material;
	Mesh::RENDERING_SHADERS renderingShaderType;

};

struct TransformData
{
	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT3> directions;
	std::vector<DirectX::XMFLOAT3> scales;
};

struct MovementData
{
	std::vector<DirectX::XMFLOAT3> translations;
	std::vector<DirectX::XMFLOAT4> rotQuats;      // rotation quaterions
	std::vector<DirectX::XMFLOAT3> scaleChanges;
};

#pragma once

#include <vector>
#include <map>
#include <string>
#include "Vertex.h"
#include "textureclass.h"
#include "../Light/LightHelper.h"

typedef unsigned int UINT;
typedef std::string MeshID;

namespace Mesh
{

	struct MeshData
	{
		std::string name{ "some_mesh" };
		std::vector<VERTEX> vertices;
		std::vector<UINT> indices;
		std::map<aiTextureType, TextureClass*> textures;
	};

	enum RENDERING_SHADERS
	{
		COLOR_SHADER,
		TEXTURE_SHADER,
		LIGHT_SHADER
	};

	struct MeshDataForRendering
	{
		ID3D11Buffer* const* ppVertexBuffer = nullptr;
		ID3D11Buffer* pIndexBuffer = nullptr;
		UINT* pStride = nullptr;
		UINT indexCount = 0;
		UINT dataIdx = 0;
		RENDERING_SHADERS renderingShaderType = COLOR_SHADER;
		D3D11_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		Material material;
	};

}

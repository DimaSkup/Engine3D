#pragma once

#include <vector>
#include <map>
#include <string>
#include "Vertex.h"
#include "textureclass.h"
#include "../Light/LightHelper.h"
#include "RenderingShaderHelperTypes.h"

typedef unsigned int UINT;
typedef std::string MeshID;

namespace Mesh
{
	enum MeshType
	{
		Plane,
		Skull,
		Cylinder,
		Cube,
		Pyramid,
		Sphere,
	};

	struct MeshData
	{
		MeshID name{ "some_mesh" };
		std::vector<VERTEX> vertices;
		std::vector<UINT> indices;
		std::map<aiTextureType, TextureClass*> textures;      // 'texture_type' => 'ptr_to_texture_obj'
		Material material;
		RENDERING_SHADERS renderingShaderType = COLOR_SHADER;
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





	// **************************************
	// DATA STRUCTURES FOR PARAMS OF BASIC MESHES 
	// **************************************
	struct MeshGeometryParams {};

	struct WavesMeshParams : public MeshGeometryParams
	{
		float spatialStep;
		float timeStep;
		float speed;
		float damping;
		UINT numRows;
		UINT numColumns;
	};

	struct CylinderMeshParams : public MeshGeometryParams
	{
		float height;
		float bottomRadius;
		float topRadius;
		UINT sliceCount;
		UINT stackCount;
	};

	struct SphereMeshParams : public MeshGeometryParams
	{
		float radius;
		UINT sliceCount;
		UINT stackCount;
	};

	struct GeosphereMeshParams : public MeshGeometryParams
	{
		float radius;
		UINT numSubdivisions;
	};

	struct PyramidMeshParams : public MeshGeometryParams
	{
		float height;
		float baseWidth;  // size of pyramid base by X
		float baseDepth;  // size of pyramid base by Z
	};

}

#pragma once

#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <string>
#include "Vertex.h"
#include "textureclass.h"
#include "../Light/LightHelper.h"
//#include "RenderingShaderHelperTypes.h"

typedef unsigned int UINT;
typedef std::string MeshID;

namespace Mesh
{
	enum MeshType
	{
		Cube,
		Cylinder,
		Plane,
		Pyramid,
		Skull,
		Sphere,
	};

	static std::map<MeshType, MeshID> basicTypeToMeshID
	{
		{ MeshType::Cube, "cube" },
		{ MeshType::Cylinder, "cylinder" },
		{ MeshType::Plane, "plane" },
		{ MeshType::Pyramid, "pyramid" },
		{ MeshType::Skull, "skull" },
		{ MeshType::Sphere, "sphere" },
	};

	struct MeshData
	{
		MeshID name{ "some_mesh" };
		std::vector<VERTEX> vertices;
		std::vector<UINT> indices;
		std::unordered_map<aiTextureType, TextureClass*> textures;      // 'texture_type' => 'ptr_to_texture_obj'
		Material material;
		
	};

	struct DataForRendering
	{
		ID3D11Buffer* const* ppVertexBuffer = nullptr;
		ID3D11Buffer* pIndexBuffer = nullptr;
		UINT* pStride = nullptr;
		UINT indexCount = 0;
		UINT dataIdx = 0;
		std::unordered_map<aiTextureType, TextureClass*> textures;      // 'texture_type' => 'ptr_to_texture_obj'
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

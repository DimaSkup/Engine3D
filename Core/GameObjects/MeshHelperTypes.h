#pragma once

#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <string>
#include <DirectXCollision.h>
#include "Vertex.h"
#include "textureclass.h"
#include "../Light/LightHelper.h"


/////////////////////////////
// TYPEDEFS
/////////////////////////////
using UINT = unsigned int;
using MeshName = std::string;
using MeshPath = std::string;
using MeshID = size_t;



/////////////////////////////
// MESH ENUMS/STRUCTURES
/////////////////////////////
namespace Mesh
{
	enum MeshType
	{
		Invalid,
		Cube,
		Cylinder,
		Plane,
		Pyramid,
		Skull,
		Sphere,
		GeoSphere,
		Imported,    // if we load a mesh from the file
	};

	static std::map<MeshType, MeshName> basicTypeToName
	{
		{ MeshType::Cube, "cube" },
		{ MeshType::Cylinder, "cylinder" },
		{ MeshType::Plane, "plane" },
		{ MeshType::Pyramid, "pyramid" },
		{ MeshType::Skull, "skull" },
		{ MeshType::Sphere, "sphere" },
	};

	// is used during generation/loading mesh
	struct MeshData
	{
		MeshType type = MeshType::Invalid;
		MeshName name{ "invalid_mesh_name" };
		MeshPath path{ "invalid_path_to_mesh" }; // where this mesh will be stored

		std::vector<VERTEX> vertices;
		std::vector<UINT> indices;

		// note: idx into this array means an aiTextureType code 
		// (for instance: if idx == 1 it means that the ptr by 
		// this idx has an aiTextureType_DIFFUSE)
		std::vector<TextureClass*> textures;      

		// note: if type of this mesh is sphere/geosphere/etc. we convert this 
		// bounding box into the bounding sphere during the frustum culling test
		DirectX::BoundingBox AABB;         

		Material material;
	};

	
	// is used to get mesh data to prepare it for rendering
	struct DataForRendering
	{
		MeshName name;  // for debug
		MeshPath path;  // for debug

		ID3D11Buffer* const* ppVertexBuffer = nullptr;
		ID3D11Buffer* pIndexBuffer = nullptr;
		UINT* pStride = nullptr;
		UINT indexCount = 0;
		UINT dataIdx = 0;
		std::vector<TextureClass*> textures; // for info look at MeshData structure
		DirectX::BoundingBox boundingBox;    // for info look at MeshData structure
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
		CylinderMeshParams(int) {}

		float height = 3;
		float bottomRadius = 0.5f;
		float topRadius = 0.3f;
		UINT sliceCount = 10;
		UINT stackCount = 10;
	};

	struct SphereMeshParams : public MeshGeometryParams
	{
		SphereMeshParams(int) {}

		float radius = 0.5f;
		UINT sliceCount = 10;
		UINT stackCount = 10;
	};

	struct GeosphereMeshParams : public MeshGeometryParams
	{
		GeosphereMeshParams(int) {}

		float radius = 0.5f;
		UINT numSubdivisions = 10;   // defatization level
	};

	struct PyramidMeshParams : public MeshGeometryParams
	{
		PyramidMeshParams(int) {}

		float height = 10;
		float baseWidth = 5;         // size of pyramid base by X
		float baseDepth = 5;         // size of pyramid base by Z
	};
}

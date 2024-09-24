#pragma once

#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <string>
#include <DirectXCollision.h>

#include "../Common/Types.h"
#include "Vertex.h"
#include "TextureHelperTypes.h"



//
// MESH TYPEDEFS
//
using UINT = unsigned int;
using MeshName = std::string;
using MeshPath = std::string;
using MeshID = uint32_t;


//
// MESH ENUMS/STRUCTURES
//
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

	struct Material
	{
		XMFLOAT4 ambient_;
		XMFLOAT4 diffuse_;
		XMFLOAT4 specular_;   // w = specPower (specular power)
		XMFLOAT4 reflect_;


		Material() :
			ambient_{ 1,1,1,1 },
			diffuse_{ 1,1,1,1 },
			specular_{ 0,0,0,0 },
			reflect_{ 0.5f, 0.5f, 0.5f, 1 } {}

		Material(
			const DirectX::XMFLOAT4& ambient,
			const DirectX::XMFLOAT4& diffuse,
			const DirectX::XMFLOAT4& specular,
			const DirectX::XMFLOAT4& reflect) :
			ambient_(ambient),
			diffuse_(diffuse),
			specular_(specular),
			reflect_(reflect) {}


		// -----------------------------------------------------------------------
		inline void SetAmbient(const DirectX::XMFLOAT4& newAmbient)
		{
			ambient_ = newAmbient;
		}
		// -----------------------------------------------------------------------
		inline void SetDiffuse(const DirectX::XMFLOAT4& newDiffuse)
		{
			diffuse_ = newDiffuse;
		}
		// -----------------------------------------------------------------------
		inline void SetSpecular(const DirectX::XMFLOAT4& newSpecular)
		{
			specular_ = newSpecular;
		}
		// -----------------------------------------------------------------------
		inline void SetSpecularPower(const float power)
		{
			specular_.w = power;
		}
		// -----------------------------------------------------------------------
	};

	static const XMFLOAT4 defaultMaterialAmbient(0.3f, 0.3f, 0.3f, 1);
	static const XMFLOAT4 defaultMaterialDiffuse(0.8f, 0.8f, 0.8f, 1);
	static const XMFLOAT4 defaultMaterialSpecular(0, 0, 0, 1);  // w == spec power
	static const XMFLOAT4 defaultMaterialReflect(0.5f, 0.5f, 0.5f, 1);

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
		std::vector<TexID> texIDs;      

		// note: if type of this mesh is sphere/geosphere/etc. we convert this 
		// bounding box into the bounding sphere during the frustum culling test
		DirectX::BoundingBox AABB;         

		Mesh::Material material;
	};

	
	// is used to get mesh data to prepare it for rendering
	struct DataForRendering
	{
		void Reserve(const u32 meshesCount)
		{
			names_.reserve(meshesCount);
			pVBs_.reserve(meshesCount);
			pIBs_.reserve(meshesCount);
			indexCount_.reserve(meshesCount);
			dataIdxs_.reserve(meshesCount);
			boundBoxes_.reserve(meshesCount);
			materials_.reserve(meshesCount);
		}

		using SRVsArr = std::vector<ID3D11ShaderResourceView*>;
		using AABB = DirectX::BoundingBox;

		std::vector<MeshName> names_;            // for debug
		std::vector<ID3D11Buffer*> pVBs_;        // ptrs to vertex buffers
		std::vector<ID3D11Buffer*> pIBs_;        // ptrs to index buffers

		//UINT stride_ = 0;
		std::vector<UINT>     indexCount_;
		std::vector<UINT>     dataIdxs_;         // for debug: mesh data idx
		std::vector<AABB>     boundBoxes_;
		std::vector<Material> materials_;

		std::vector<TexID> texIDs_;           // array of texture IDs for each mesh
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

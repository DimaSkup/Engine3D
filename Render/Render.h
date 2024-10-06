// *********************************************************************************
// Filename:     Render.h
// Description:  this class is responsible for rendering all the 
//               graphics onto the screen;
// Created:      02.12.22 (moved into the Render module at 29.08.24)
// *********************************************************************************
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "Shaders/ShadersContainer.h"
#include "Shaders/Helpers/LightHelperTypes.h"

#include <d3d11.h>
#include <DirectXMath.h>


namespace Render
{


//////////////////////////////////
// Class name: Render
//////////////////////////////////
class Render final
{
public:
	struct RenderParams
	{
		DirectX::XMMATRIX WVO;        // is used for 2D rendering (world * basic_view * ortho)
		DirectX::XMMATRIX proj;
		DirectX::XMMATRIX viewProj;   // view * projection
		DirectX::XMFLOAT3 cameraPos;
		DirectX::XMFLOAT3 cameraDir;  // the direction where the camera is looking at

		float deltaTime = 0;          // time passed since the previous frame
		float totalGameTime = 0;      // time passed since the start of the application

	} params_;

	struct PerFrameData
	{
		DirectX::XMMATRIX viewProj;    // (is already transposed)
		DirectX::XMFLOAT3 cameraPos;
		std::vector<DirLight>   dirLights;
		std::vector<PointLight> pointLights;
		std::vector<SpotLight>  spotLights;

		void Clear()
		{
			dirLights.clear();
			pointLights.clear();
			spotLights.clear();
		}
	};

	struct InstanceBufferData
	{
		std::vector<DirectX::XMMATRIX> worlds;
		std::vector<DirectX::XMMATRIX> texTransforms;
		std::vector<Material> meshesMaterials;

		void Clear()
		{
			worlds.clear();
			texTransforms.clear();
			meshesMaterials.clear();
		}
	};

	struct InstancesDataToRender
	{
		std::vector<ID3D11Buffer*> ptrsMeshVB;                     // arr of ptrs to meshes vertex buffers
		std::vector<ID3D11Buffer*> ptrsMeshIB;                     // arr of ptrs to meshes index buffers
		std::vector<ID3D11ShaderResourceView*> texturesSRVs;       // each set of instances has own set of textures
		std::vector<ptrdiff_t> numInstancesPerMesh;               // how many instances will be rendered using this geometry
		std::vector<uint32_t> enttsMaterialTexIdxs;               // arr of idxs to the sets of texturesSRVs
		std::vector<uint32_t> enttsPerTexSet;                     // how many instances will be rendered using the current texture set (they can have different textures but the same geometry)
		std::vector<uint32_t> indexCounts;                        // index count for the current geometry
		uint32_t numOfTexSet;                                      // how many textures sets we have
		uint32_t vertexSize;

		void Clear()
		{
			ptrsMeshVB.clear();
			ptrsMeshIB.clear();
			texturesSRVs.clear();
			numInstancesPerMesh.clear();
			enttsMaterialTexIdxs.clear();
			enttsPerTexSet.clear();
			indexCounts.clear();
			numOfTexSet = 0;
			vertexSize = 0;
		}
	};

public:
	Render();
	~Render();

	// restrict a copying of this class instance
	Render(const Render& obj) = delete;
	Render& operator=(const Render& obj) = delete;


	// initialize the rendering subsystem
	bool Initialize(
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX& worldViewOrtho);

	void UpdatePerFrame(ID3D11DeviceContext* pDeviceContext, const PerFrameData& data);
	void UpdateInstancedBuffer(ID3D11DeviceContext* pDeviceContext_, const InstanceBufferData& data);

	void RenderInstances(
		ID3D11DeviceContext* pDeviceContext,
		const InstancesDataToRender& instancesData,
		const std::vector<ID3D11Buffer*>& ptrsMeshVB,                     // arr of ptrs to meshes vertex buffers
		const std::vector<ID3D11Buffer*>& ptrsMeshIB,                     // arr of ptrs to meshes index buffers
		const std::vector<uint32_t>& indexCounts);
		
	bool Render3D();

	inline ShadersContainer& GetShadersContainer() { return shadersContainer_; }
	inline LightShaderClass& GetLightShader() { return shadersContainer_.lightShader_; }

public:
	PerFrameData perFrameData_;      // we need to keep this data because we use it multiple times during the frame


private:
	// render all the 2D / 3D models onto the screen
	void RenderModels();

private:
	ShadersContainer shadersContainer_;          // a struct with shader classes objects

};

}; // namespace Render
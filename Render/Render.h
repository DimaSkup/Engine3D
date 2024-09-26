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

public:
	Render();
	~Render();

	// initialize the rendering subsystem
	bool Initialize(
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX& worldViewOrtho);

	bool UpdatePerFrame(
		ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX& viewProj,    // (is already transposed)
		const DirectX::XMFLOAT3& cameraPos,
		const std::vector<DirLight>& dirLights,
		const std::vector<PointLight>& pointLights,
		const std::vector<SpotLight>& spotLights);

	bool UpdateInstancedBuffer(
		ID3D11DeviceContext* pDeviceContext_,
		const std::vector<DirectX::XMMATRIX>& worlds,
		const std::vector<DirectX::XMMATRIX>& texTransforms,
		const std::vector<Material>& meshesMaterials);

	bool RenderInstances(
		ID3D11DeviceContext* pDeviceContext,
		std::vector<ID3D11Buffer*>& ptrsMeshVB,                     // arr of ptrs to meshes vertex buffers
		std::vector<ID3D11Buffer*>& ptrsMeshIB,                     // arr of ptrs to meshes index buffers
		const std::vector<ID3D11ShaderResourceView*>& texturesSRVs, 
		const std::vector<ptrdiff_t>& numInstancesPerMesh,
		const std::vector<uint32_t>& instancesCountsPerTexSet,          // the same geometry can have different textures;
		const std::vector<uint32_t>& indexCounts,
		const uint32_t vertexSize);

	bool Render3D();

	inline ShadersContainer& GetShadersContainer() { return shadersContainer_; }
	inline LightShaderClass& GetLightShader() { return shadersContainer_.lightShader_; }


private:  // restrict a copying of this class instance
	Render(const Render& obj);
	Render& operator=(const Render& obj);

private:
	// render all the 2D / 3D models onto the screen
	void RenderModels();

private:
	ShadersContainer shadersContainer_;          // a struct with shader classes objects

};

}; // namespace Render
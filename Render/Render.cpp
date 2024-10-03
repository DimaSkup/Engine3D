////////////////////////////////////////////////////////////////////
// Filename:     Render.cpp
// Description:  there are functions for rendering graphics;
// Created:      01.01.23
// Revising:     01.01.23
////////////////////////////////////////////////////////////////////
#include "Render.h"

//#include "Common/MathHelper.h"
//#include "Common/Utils.h"
#include "Common/log.h"
#include "InitRender.h"

//#include <DirectXCollision.h>

namespace Render
{


Render::Render()
{
	//Log::Debug();
}

Render::~Render()
{
	//Log::Debug();
}




////////////////////////////////////////////////////////////////////////////////////////////
//                             PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////



bool Render::Initialize(
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX& worldViewOrtho) 
{
	try
	{
		//Log::Debug();

		bool result = true;
		InitRender init;

		result = init.InitializeShaders(
			pDevice, 
			pDeviceContext,
			shadersContainer_,
			worldViewOrtho);
		Assert::True(result, "can't initialize shaders");
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, true);
		Log::Error("can't initialize the Render module");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

void Render::UpdatePerFrame(
	ID3D11DeviceContext* pDeviceContext,
	const PerFrameData& data)
{
	try 
	{
		shadersContainer_.lightShader_.UpdatePerFrame(
			pDeviceContext,
			data.viewProj,
			data.cameraPos,
			data.dirLights,
			data.pointLights,
			data.spotLights,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e);
	}
	catch (...)
	{
		Log::Error("something went wrong during updating data for rendering");
	}
}


///////////////////////////////////////////////////////////

void Render::UpdateInstancedBuffer(
	ID3D11DeviceContext* pDeviceContext_,
	const InstanceBufferData& data)
{
	try
	{
		shadersContainer_.lightShader_.UpdateInstancedBuffer(
			pDeviceContext_,
			data.worlds,
			data.texTransforms,
			data.meshesMaterials);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e);
		Log::Error("can't update instanced buffer for rendering");
	}
	catch (...)
	{
		Log::Error("can't update instanced buffer for rendering for some unknown reason :)");
	}
}

///////////////////////////////////////////////////////////

void Render::RenderInstances(
	ID3D11DeviceContext* pDeviceContext,
	const InstancesDataToRender& instancesData,
	const std::vector<ID3D11Buffer*>& ptrsMeshVB,                     // arr of ptrs to meshes vertex buffers
	const std::vector<ID3D11Buffer*>& ptrsMeshIB,                     // arr of ptrs to meshes index buffers
	const std::vector<uint32_t>& indexCounts)
{
	try
	{
		shadersContainer_.lightShader_.Render(
			pDeviceContext,
			ptrsMeshVB,
			ptrsMeshIB,
			instancesData.texturesSRVs,
			instancesData.numInstancesPerMesh,
			instancesData.enttsMaterialTexIdxs,
			instancesData.enttsPerTexSet,
			indexCounts,
			instancesData.numOfTexSet,
			instancesData.vertexSize);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e);
		Log::Error("can't render the mesh instances onto the screen");
	}
	catch (...)
	{
		Log::Error("can't render mesh instances for some unknown reason :)");
	}
}

///////////////////////////////////////////////////////////

}; // namespace Render
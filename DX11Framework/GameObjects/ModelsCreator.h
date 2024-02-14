////////////////////////////////////////////////////////////////////////////////////////////
// Filename:        ModelsCreator.h
// Description:     a functional for models creation
//                  (plane, cube, imported models, etc.)
//
// Created:         12.02.24
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>     
#include <windows.h>  // for using UINT type
#include "ModelsStore.h"

class ModelsCreator
{
public:
	const UINT CreatePlane(ID3D11Device* pDevice,
		ModelsStore & modelsStore,
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection);

	const UINT CreateCube(ID3D11Device* pDevice, 
		ModelsStore & modelsStore,
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection);



	const UINT CreateCopyOfModelByIndex(const UINT index,
		ModelsStore & modelsStore,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMVECTOR & position,
		const DirectX::XMVECTOR & rotation);
};
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <memory>   // for std::shared_ptr

#if 0

#include "../Engine/Settings.h"
#include "../Model/TerrainCellClass.h"
#include "../Model/RenderableGameObject.h"





class TerrainInitializerInterface
{
public:
	virtual bool Initialize(Settings* pSettings,
		std::shared_ptr<TerrainSetupData> pTerrainSetupData,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		std::vector<RenderableGameObject*> & terrainCellsArr,
		ModelInitializerInterface* pModelInitializer,
		ModelToShaderMediatorInterface* pModelToShaderMediator) = 0;
};


#endif
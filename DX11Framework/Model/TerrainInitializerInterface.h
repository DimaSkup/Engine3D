#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <memory>   // for std::shared_ptr

#include "../Engine/Settings.h"
#include "../Model/TerrainCellClass.h"
#include "../Model/RenderableGameObject.h"


struct TerrainSetupData
{
	std::string terrainFilename{ "" };     // a name of the terrain height map file
	std::string colorMapFilename{ "" };    // a name of the colour map file
	std::string renderingShaderName{ "" }; // a name of the shader which will be used for rendering of the terrain

	UINT terrainHeight = 0;
	UINT terrainWidth = 0;
	UINT cellCount = 0;                    // a count variable to keep track how many cells are in the terrain
	float heightScale = 0.0f;              // a degree of smoothing of the terrain
	
};


struct HeightMapType
{
	HeightMapType()
		: position{ 0.0f, 0.0f, 0.0f },
		  normal{ 0.0f, 0.0f, 0.0f },
		  color{ 1.0f, 1.0f, 1.0f, 1.0f }
	{
	}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 color;   // RGBA color
};



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
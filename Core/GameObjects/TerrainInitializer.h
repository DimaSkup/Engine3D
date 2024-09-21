#pragma once

#include <fstream>
#include <vector>

#include <DirectXMath.h>

typedef unsigned int UINT;

class TerrainInitializer
{
	
public: 
	////////////////////////////////////////////////////////////////////////////////////////
	//               DATA STRUCTURES FOR TERRAIN PROPERTIES AND SETUP DATA
	////////////////////////////////////////////////////////////////////////////////////////

	struct TerrainSetupData
	{
		std::string terrainFilename{ "" };     // a name of the terrain height map file
		std::string colorMapFilename{ "" };    // a name of the colour map file
		std::string renderingShaderName{ "" }; // a name of the shader which will be used for rendering of the terrain

		UINT terrainWidth = 0;
		UINT terrainDepth = 0;
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




	
public:
	////////////////////////////////////////////////////////////////////////////////////////
	//                                PUBLIC API 
	////////////////////////////////////////////////////////////////////////////////////////

	bool LoadSetupFile(const std::string & setupFilePath);
	inline const TerrainSetupData & GetSetupData() const { return setupData_; };

private:
	TerrainSetupData setupData_;
};

#if 0

#include "TerrainInitializerInterface.h"
#include "TerrainCellClass.h"

#include "../Engine/log.h"
#include "../Model/Vertex.h"
#include "../Model/ModelMath.h"

class TerrainInitializer : public TerrainInitializerInterface
{
public:
	virtual bool Initialize(Settings* pSettings, 
		std::shared_ptr<TerrainSetupData> pTerrainSetupData,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		std::vector<RenderableGameObject*> & terrainCellsArr,
		ModelInitializerInterface* pModelInitializer,
		ModelToShaderMediatorInterface* pModelToShaderMediator) override;


private:
	void Shutdown();  // clear memory after initialization of the terrain

	
	void LoadBitmapHeightMap();  // the function for loading the height map into the height map array
	bool LoadRawHeightMap();     // a function for loading 16bit RAW height maps

	void SetupTerrainCoordinates();

	bool CalculateNormals();
	void CalculateFacesNormals(std::vector<DirectX::XMFLOAT3> & normalsArr);
	void ComputeTerrainVectors();  // the function for computation the tagnent and binormal for the terrain model

	void LoadColorMap();        // the function for loading the color map into the height map array
	bool BuildTerrainModel();   // the function for building the terrain vertices
	bool BuildTerrainModel_OptimizedVersion();

	bool LoadTerrainCells(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		std::vector<RenderableGameObject*> & terrainCellsArr,
		ModelInitializerInterface* pModelInitializer,
		ModelToShaderMediatorInterface* pModelToShaderMediator);

	void SkipUntilSymbol(std::ifstream & fin, char symbol);  // go through input stream while we don't find a particular symbol

private:
	std::vector<HeightMapType> heightMapArr_;          // a pointer to the height map data array
	std::shared_ptr<TerrainSetupData> pSetupData_;
	std::vector<VERTEX> verticesArr_;
	std::vector<UINT> indicesArr_;

	// set the height and width of each terrain cell to some height_x_width vertex array (for example: 33x33)
	UINT cellWidth_ = 0;
	UINT cellHeight_ = 0;    
};


#endif
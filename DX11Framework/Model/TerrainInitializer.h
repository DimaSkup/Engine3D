#pragma once

#include <fstream>
#include <vector>

#include "TerrainInitializerInterface.h"
#include "TerrainCellClass.h"

#include "../Engine/log.h"
#include "../Engine/macros.h"
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

	bool LoadSetupFile(const std::string & setupFilePath, std::shared_ptr<TerrainSetupData> pSetupData);
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
////////////////////////////////////////////////////////////////////
// Filename::    TerrainClass.h
// Description:  the terrain class will encapsulate the model data and 
//               rendering functionality for drawing the terrain (or terrain grid)
//
// Created:      11.03.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>

#include "../Engine/log.h"
#include "../Model/Vertex.h"
#include "../Model/modelclass.h"
#include "../Model/modellistclass.h"


//////////////////////////////////
// Class name: TerrainClass
//////////////////////////////////
class TerrainClass : public ModelClass
{
private:
	struct HeightMapType
	{
		float x, y, z;
	};

	struct ModelType
	{
		float x, y, z;
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass& copy);
	~TerrainClass();

	virtual bool Initialize(ID3D11Device* pDevice) override;
	virtual void Render(ID3D11DeviceContext* pDeviceContext) override;

private:
	void CreateTerrainData();

	bool LoadSetupFile(char* filepath);
	bool LoadBitmapHeightMap();
	void ShutdownHeightMap();

	void SetTerrainCoordinates();
	bool BuildTerrainModel();
	void ShutdownTerrainModel();

private:
	void SkipUntilSymbol(ifstream & fin, char symbol);

private:
	std::string modelType_{ "terrain" };

	UINT terrainHeight_ = 0;
	UINT terrainWidth_ = 0;
	float heightScale_ = 0.0f;
	char* terrainFilename_ = nullptr;
	HeightMapType* pHeightMap_ = nullptr;
	//ModelType* pModel_ = nullptr;
};

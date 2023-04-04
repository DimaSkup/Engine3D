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
		HeightMapType()
		{
			x = y = z = 0.0f;
			nx = ny = nz = 0.0f;
		}

		float x, y, z;
		float nx, ny, nz;
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass& copy);
	~TerrainClass();

	virtual bool Initialize(ID3D11Device* pDevice) override;
	virtual void Render(ID3D11DeviceContext* pDeviceContext) override;

	float GetWidth() const;
	float GetHeight() const;

private:
	bool LoadSetupFile(const char* filepath);
	bool LoadBitmapHeightMap();

	void SetTerrainCoordinates();
	bool CalculateNormals();
	bool BuildTerrainModel();

	void SkipUntilSymbol(ifstream & fin, char symbol);  // go through input stream while we don't find a particular symbol

private:
	std::string modelType_{ "terrain" };

	UINT terrainHeight_ = 0;
	UINT terrainWidth_ = 0;
	float heightScale_ = 0.0f;
	char* terrainFilename_ = nullptr;
	HeightMapType* pHeightMap_ = nullptr;
};

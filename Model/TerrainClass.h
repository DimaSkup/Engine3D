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
			: position{ 0.0f, 0.0f, 0.0f },
			  normal {0.0f, 0.0f, 0.0f}
		{
		}

		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
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
	void CalculateFacesNormals(DirectX::XMFLOAT3* pNormals);
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

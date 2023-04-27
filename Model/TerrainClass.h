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
	struct RGB_COLOR
	{
		RGB_COLOR(float red, float green, float blue) :
			r(red), g(green), b(blue) {}

		float r = 1.0f;
		float g = 1.0f;
		float b = 1.0f;
	};

	struct HeightMapType
	{
		HeightMapType() 
			: position{ 0.0f, 0.0f, 0.0f },
			  normal { 0.0f, 0.0f, 0.0f },
			  color { 1.0f, 1.0f, 1.0f }
		{
		}

		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		RGB_COLOR color;
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
	bool LoadBitmapHeightMap();  // the function for loading the height map into the height map array
	bool LoadRawHeightMap();     // a function for loading 16bit RAW height maps

	void SetTerrainCoordinates();
	bool CalculateNormals();
	void CalculateFacesNormals(DirectX::XMFLOAT3* pNormals);

	bool LoadColorMap();        // the function for loading the color map into the height map array
	bool BuildTerrainModel();   // the function for building the terrain vertices
	void CalculateTerrainVectors();  // the function for calculating the tagnent and binormal for the terrain model

	void SkipUntilSymbol(ifstream & fin, char symbol);  // go through input stream while we don't find a particular symbol

private:
	std::string modelType_{ "terrain" };

	UINT terrainHeight_ = 0;
	UINT terrainWidth_ = 0;
	float heightScale_ = 0.0f;
	char* terrainFilename_ = nullptr;
	char* colorMapFilename_ = nullptr;
	HeightMapType* pHeightMap_ = nullptr;
};

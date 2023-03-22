////////////////////////////////////////////////////////////////////
// Filename::    TerrainClass.cpp
// Description:  the terrain class will encapsulate the model data and 
//               rendering functionality for drawing the terrain (or terrain grid)
//
// Created:      11.03.23
////////////////////////////////////////////////////////////////////
#include "TerrainClass.h"



TerrainClass::TerrainClass()
{
}

TerrainClass::TerrainClass(const TerrainClass& copy)
{
}

TerrainClass::~TerrainClass()
{
	ShutdownTerrainModel(); // release the terrain model
	ShutdownHeightMap();    // release the height map
}


////////////////////////////////////////////////////////////////////
//
//                     PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////

// the Inialize() function will just call the functions for initializing the 
// vertex and index buffers that will hold the terrain data
bool TerrainClass::Initialize(ID3D11Device* pDevice)
{
	bool result = false;
	ModelListClass* pModelList = ModelListClass::Get();
	char* setupFilename = "data/terrain/setup.txt";

	// get the terrain filename, dimensions, and so forth from the setup file
	result = LoadSetupFile(setupFilename);
	COM_ERROR_IF_FALSE(result, "can't load the setup file");

	// initialize the terrain height map with the data from the bitmap file
	result = LoadBitmapHeightMap();
	COM_ERROR_IF_FALSE(result, "can't load the bitmap height map");

	// setup the X and Z coordinates for the height map as well as scale the terrain
	// height by the height scale value
	SetTerrainCoordinates();

	// now build the 3D model of the terrain
	result = BuildTerrainModel();
	COM_ERROR_IF_FALSE(result, "can't build a terrain model");

	// we can now release the height map since it is no longer seeded in memory once
	// the 3D terrain model has been built
	ShutdownHeightMap();

	// load the rendering buffers with the terrain data
	result = this->InitializeBuffers(pDevice);
	COM_ERROR_IF_FALSE(result, "can't intialize buffers for the terrain grid");

	// release the terrain model now that the rendering buffers have been loaded
	ShutdownTerrainModel();

	// setup the id of the model
	SetID(modelType_);

	// print a message about the initialization process
	string debugMsg = modelType_ + " is initialized!";
	Log::Debug(THIS_FUNC, debugMsg.c_str());

	return true;
}


// Put the vertex buffer data and index buffer data on the video card 
// to prepare this data for rendering
void TerrainClass::Render(ID3D11DeviceContext* pDeviceContext)
{
	this->RenderBuffers(pDeviceContext);

	// the single difference here is that we render buffers using another type of the primitive topology;
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	pMediator_->Render(pDeviceContext);

	return;
}



////////////////////////////////////////////////////////////////////
//
//                    PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////

// LoadSetupFile() is a function that takes in the terrain setup file and stores
// all the values so that we can construct the terrain based on what is in that file.
// Reads the bitmap height map file, the terrain width and height, the terrain
// height scaling value, etc.
bool TerrainClass::LoadSetupFile(char* filename)
{
	assert(filename != nullptr);

	int stringLength = 256;  
	ifstream fin;
	char input = ' ';
	//bool result = false;

	// initialize the string that will hold the terrain file name
	terrainFilename_ = new char[stringLength];
	COM_ERROR_IF_FALSE(terrainFilename_, "can't allocate memory for the terrain filename");

	// open the setup file. If it could not open the file then exit
	fin.open(filename);
	if (fin.fail())
	{
		COM_ERROR_IF_FALSE(false, "can't open the setup file");
	}

	SkipUntilSymbol(fin, ':'); // read up to the terrain filename
	fin >> terrainFilename_;   // read in the terrain file name

	
	SkipUntilSymbol(fin, ':'); // read up to the value of terrain height
	fin >> terrainHeight_;     // read in the terrain height

	
	SkipUntilSymbol(fin, ':'); // read up to the value of terrain width
	fin >> terrainWidth_;      // read in the terrain width

	
	SkipUntilSymbol(fin, ':'); // read up to the value of the terrain height scaling
	fin >> heightScale_;       // read in the terrain height scaling
	 
	// close the setup file
	fin.close();

	return true;
}



// LoadBitmapHeightMap() loads the bitmap file containing the height map into the 
// height map array. Note that the bitmap format contains red, green, and blue colours.
// But since this being treated like a grey scale image you can read either the red, green,
// or blue colour as the will all be the same grey value and you only need one of them.
// Also note that we use odd width_x_height of the bitmap because we need an odd number of 
// points to build an even number of quads. And finally the bitmap format stores the
// image upside down. And because of this we first need to read the data into an array,
// and then copy that array into the height map from the bottom up.
bool TerrainClass::LoadBitmapHeightMap()
{
	errno_t error = 0;
	UINT imageSize = 0;
	FILE* filePtr = nullptr;
	size_t count = 0;
	size_t index = 0;              // a height map position index
	size_t imgBufferPos = 0;   // initialize the position in the image data buffer
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	UCHAR* pBitmapImage;
	UCHAR height = 0;

	// start by creating the array structure to hold the height map data
	pHeightMap_ = new HeightMapType[terrainWidth_ * terrainHeight_];
	COM_ERROR_IF_FALSE(pHeightMap_, "can't allocate memory for a height map array");

	// open the bitmap map file in binary
	error = fopen_s(&filePtr, terrainFilename_, "rb");
	COM_ERROR_IF_FALSE(error == 0, "can't open the bitmap map file");

	// read in the bitmap file header
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	COM_ERROR_IF_FALSE(count == 1, "can't read in the bitmap file header");

	// read in the bitmap info header
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	COM_ERROR_IF_FALSE(count == 1, "can't read in the bitmap info header");

	// make sure the height map dimensions are the same as the terrain dimensions
	// for easy 1 to 1 mapping
	if ((bitmapInfoHeader.biHeight != terrainHeight_) || (bitmapInfoHeader.biWidth != terrainWidth_))
		COM_ERROR_IF_FALSE(false, "map dimensions are no the same as the terrain dimensions");

	// calculate the size of the bitmap image data;
	// since we use non-divide by 2 dimensions (eg. 257x257) we need to add 
	// an extra byte to each line.
	imageSize = terrainHeight_ * ((terrainWidth_ * 3) + 1);

	// allocate memory for the bitmap image data
	pBitmapImage = new UCHAR[imageSize];
	COM_ERROR_IF_FALSE(pBitmapImage, "can't allocate memory for the bitmap image data");

	// move to the beginning of the bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// read in the bitmap image data
	count = fread(pBitmapImage, 1, imageSize, filePtr);
	COM_ERROR_IF_FALSE(count == imageSize, "can't read in the bitmap image data");

	// close the file
	error = fclose(filePtr);
	COM_ERROR_IF_FALSE(error == 0, "can't close the file");

	// read the image data into the height map array
	for (size_t j = 0; j < terrainHeight_; j++)
	{
		for (size_t i = 0; i < terrainWidth_; i++)
		{
			// bitmaps are upside down so load bottom to top into the height map array
			index = (terrainWidth_ * (terrainHeight_ - 1 - j)) + i;

			// get the grey scale pixel value from the bitmap image data at this location
			height = pBitmapImage[imgBufferPos];

			// store the pixel value as the height at this point in the height map array
			pHeightMap_[index].y = static_cast<float>(height);

			// increment the bitmap image data index
			imgBufferPos += 3;
		}

		// compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257)
		imgBufferPos++;
	}

	// release the bitmap image data now that the height map array has been loaded
	_DELETE(pBitmapImage);

	// release the terrain filename now that it has been read in
	_DELETE(terrainFilename_);

	return true;
}







void TerrainClass::CreateTerrainData()
{
	pModelData_ = nullptr;
	pIndicesData_ = nullptr;
	int terrainWidth = 0;
	int terrainHeight = 0;
	int index = 0;                                      // intialize the index into the vertex and index arrays
	float positionX = 0.0f;
	float positionZ = 0.0f;
	DirectX::XMFLOAT4 color{ 1.0f, 1.0f, 1.0f, 1.0f };  // set the color of the terrain grid


	// set the height and width of the terrain grid
	terrainHeight = 256;
	terrainWidth = 256;

	
	// calculated the number of vertices in the terrain
	vertexCount_ = (terrainWidth - 1) * (terrainHeight - 1) * 8;

	// set the index count to the same as the vertex count
	indexCount_ = vertexCount_;

	// create the vertex array
	pModelData_ = new VERTEX[vertexCount_];
	COM_ERROR_IF_FALSE(pModelData_, "can't allocate the memory for a vertex array");

	// create the index array
	pIndicesData_ = new UINT[indexCount_];

	// load the vertex array and index array with data
	for (size_t j = 0; j < (terrainHeight - 1); j++)
	{
		for (size_t i = 0; i < (terrainWidth - 1); i++)
		{
			// Line 1 -- upper left
			positionX = static_cast<float>(i);
			positionZ = static_cast<float>(j + 1);

			pModelData_[index].position = { positionX, 0.0f, positionZ };
			pModelData_[index].color = color;
			pIndicesData_[index] = index;
			index++;

			// Line 1 -- upper right
			positionX = static_cast<float>(i + 1);
			positionZ = static_cast<float>(j + 1);

			pModelData_[index].position = { positionX, 0.0f, positionZ };
			pModelData_[index].color = color;
			pIndicesData_[index] = index;
			index++;

			// Line 2 -- upper right
			positionX = static_cast<float>(i + 1);
			positionZ = static_cast<float>(j + 1);

			pModelData_[index].position = { positionX, 0.0f, positionZ };
			pModelData_[index].color = color;
			pIndicesData_[index] = index;
			index++;

			// Line 2 -- bottom right
			positionX = static_cast<float>(i + 1);
			positionZ = static_cast<float>(j);

			pModelData_[index].position = { positionX, 0.0f, positionZ };
			pModelData_[index].color = color;
			pIndicesData_[index] = index;
			index++;

			// Line 3 -- bottom right
			positionX = static_cast<float>(i + 1);
			positionZ = static_cast<float>(j);

			pModelData_[index].position = { positionX, 0.0f, positionZ };
			pModelData_[index].color = color;
			pIndicesData_[index] = index;
			index++;

			// Line 3 -- bottom left
			positionX = static_cast<float>(i);
			positionZ = static_cast<float>(j);

			pModelData_[index].position = { positionX, 0.0f, positionZ };
			pModelData_[index].color = color;
			pIndicesData_[index] = index;
			index++;

			// Line 4 -- bottom left
			positionX = static_cast<float>(i);
			positionZ = static_cast<float>(j);

			pModelData_[index].position = { positionX, 0.0f, positionZ };
			pModelData_[index].color = color;
			pIndicesData_[index] = index;
			index++;

			// Line 4 -- upper left
			positionX = static_cast<float>(i);
			positionZ = static_cast<float>(j + 1);

			pModelData_[index].position = { positionX, 0.0f, positionZ };
			pModelData_[index].color = color;
			pIndicesData_[index] = index;
			index++;
		}
	}
}








void SkipUntilSymbol(ifstream & fin, char symbol)
{
	char input = ' ';

	fin.get(input);
	while (input != symbol)
	{
		fin.get(input);
	}
}
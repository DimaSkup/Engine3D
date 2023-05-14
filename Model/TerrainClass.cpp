////////////////////////////////////////////////////////////////////
// Filename::    TerrainClass.cpp
// Description:  the terrain class will encapsulate the model data and 
//               rendering functionality for drawing the terrain (or terrain grid)
//
// Created:      11.03.23
////////////////////////////////////////////////////////////////////
#include "TerrainClass.h"

using namespace DirectX;


TerrainClass::TerrainClass()
{
}

TerrainClass::TerrainClass(const TerrainClass& copy)
{
}

TerrainClass::~TerrainClass()
{
	Log::Print("-------------------------------------------------");
	Log::Print("            THE TERRAIN DESTROYMENT:             ");
	Log::Print("-------------------------------------------------");
	Log::Debug(THIS_FUNC_EMPTY);

	Shutdown();                     // Shutting down of the model class, releasing of the memory, etc.
	_DELETE_ARR(pTerrainCells_);    // release the terrain cells
	_DELETE_ARR(pHeightMap_);       // release the height map array
	_DELETE(terrainFilename_);      // release the terrain height map filename
	_DELETE(colorMapFilename_);     // release the terrain color map filename
	Log::Print(THIS_FUNC, "The Terrain is destroyed");
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
	Log::Debug(THIS_FUNC_EMPTY);
	assert(pDevice);
	
	bool result = false;
	ModelListClass* pModelList = ModelListClass::Get();
	const char* setupFilename{ "data/terrain/setup.txt" };

	// get the terrain filename, dimensions, and so forth from the setup file
	result = LoadSetupFile(setupFilename);
	COM_ERROR_IF_FALSE(result, "can't load the setup file");

	// initialize the terrain height map with the data from the bitmap file
	result = LoadBitmapHeightMap();
	COM_ERROR_IF_FALSE(result, "can't load the bitmap height map");

	// initialize the terrain height map with the data from the raw file
	//result = LoadRawHeightMap();
	//COM_ERROR_IF_FALSE(result, "can't load the raw height map");


	// setup the X and Z coordinates for the height map as well as scale the terrain
	// height by the height scale value
	SetTerrainCoordinates();

	// calculate the normals for the terrain data
	result = CalculateNormals();
	COM_ERROR_IF_FALSE(result, "can't calculate the normals for the terrain");

	// load in the colour map for the terrain
	result = LoadColorMap();
	COM_ERROR_IF_FALSE(result, "can't load in the colour map");

	// now build the 3D model of the terrain
	result = BuildTerrainModel();
	COM_ERROR_IF_FALSE(result, "can't build a terrain model");

	// we can now release the height map since it is no longer seeded in memory once
	// the 3D terrain model has been built
	_DELETE_ARR(pHeightMap_); // Release the height map array
	
	// calculate the tangent and binormal for the terrain model
	CalculateTerrainVectors();

	// create and load cells with the terrain data
	result = LoadTerrainCells(pDevice);
	COM_ERROR_IF_FALSE(result, "can't load terrain cells");


	// release the terrain model data now that the rendering buffers have been loaded
	ClearModelData();

	// setup the id of the model
	SetID(modelType_);

	// print a message about the initialization process
	string debugMsg = modelType_ + " is initialized!";
	Log::Debug(THIS_FUNC, debugMsg.c_str());

	return true;
}


float TerrainClass::GetWidth() const
{
	return static_cast<float>(terrainWidth_);
}

float TerrainClass::GetHeight() const
{
	return static_cast<float>(terrainHeight_);
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
bool TerrainClass::LoadSetupFile(const char* filename)
{
	Log::Debug(THIS_FUNC_EMPTY);
	assert(filename != nullptr);


	int stringLength = 256;  
	ifstream fin;
	
	// set minimal possible params for the terrain
	UINT minTerrainDimensionMagnitude = 1;
	float minHeightScale = 0.0f;

	// initialize the string that will hold the terrain file name
	terrainFilename_ = new char[stringLength] {'\0'};
	COM_ERROR_IF_FALSE(terrainFilename_, "can't allocate memory for the terrain filename");

	// initialize the string that will hold the color map file name
	colorMapFilename_ = new char[stringLength] {'\0'};
	COM_ERROR_IF_FALSE(terrainFilename_, "can't allocate memory for the color map filename");

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

	SkipUntilSymbol(fin, ':'); // read up to the value of the color map bitmap file name
	fin >> colorMapFilename_;
	
	// make confidence that we have got proper terrain values
	assert(terrainHeight_ > minTerrainDimensionMagnitude);
	assert(terrainWidth_ > minTerrainDimensionMagnitude);
	assert(heightScale_ > minHeightScale);
	assert(strlen(terrainFilename_) > 0);
	assert(strlen(colorMapFilename_) > 0);


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
	Log::Debug(THIS_FUNC_EMPTY);

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
			pHeightMap_[index].position.y = static_cast<float>(height);

			// increment the bitmap image data index
			imgBufferPos += 3;
		}

		// compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257)
		imgBufferPos++;
	}

	// release the bitmap image data now that the height map array has been loaded
	_DELETE_ARR(pBitmapImage);

	// release the terrain filename now that it has been read in
	_DELETE(terrainFilename_);

	return true;
}

// The LoadRawHeightMap() loads 16 bit RAW height map files. It works in the exact same 
// fashion as LoadBitmapHeightMap() but handles the RAW format instead. Since this is a 
// 16 bit format we use unsigned short instead of unsigned char to create the array that
// the data will be read into. Also when we parse through the array to copy the data
// into the height map structure we don't have to traverse it backwards because the RAW
// format is not stored unpside down like bitmaps.
bool TerrainClass::LoadRawHeightMap()
{
	Log::Debug(THIS_FUNC_EMPTY);

	errno_t error = 0;
	UINT index = 0;
	FILE* pFile = nullptr;  
	ULONGLONG imageSize = 0;
	ULONGLONG count = 0;
	USHORT* pRawImage = nullptr;

	try
	{
		// create the float array to hold the height map data
		pHeightMap_ = new(std::nothrow) HeightMapType[terrainWidth_ * terrainHeight_];
		COM_ERROR_IF_FALSE(pHeightMap_, "can't allocate memory for the height map array");

		// open the 16 bit raw height map file for reading in binary
		error = fopen_s(&pFile, terrainFilename_, "rb");
		COM_ERROR_IF_FALSE(error == 0, "can't open the 16 bit raw height map file");

		// calculate the size of the raw image data
		imageSize = terrainHeight_ * terrainWidth_;

		// allocate memory for the raw image data
		pRawImage = new(std::nothrow) USHORT[imageSize];
		COM_ERROR_IF_FALSE(pRawImage, "can't allocate memory for the height map array");

		// read in the raw image data
		count = fread(pRawImage, sizeof(USHORT), imageSize, pFile);
		COM_ERROR_IF_FALSE(count == imageSize, "can't read in the raw image data");

		// close the file
		error = fclose(pFile);
		COM_ERROR_IF_FALSE(error == 0, "can't close the file");

		// copy the image data into the height map array
		for (UINT j = 0; j < terrainHeight_; j++)
		{
			for (UINT i = 0; i < terrainWidth_; i++)
			{
				index = (terrainWidth_ * j) + i;

				// store the height at this point in the height map array
				pHeightMap_[index].position.y = static_cast<float>(pRawImage[index]);
			}
		}

		
		_DELETE_ARR(pRawImage);        // release the 16bit raw height map data
		_DELETE(terrainFilename_);     // release the terrain filename now that it has been read in
	}
	catch (COMException & e)
	{
		_DELETE_ARR(pHeightMap_);
		_DELETE_ARR(pRawImage);
		Log::Error(e);
		return false;
	}

	return true;
}


// The SetTerrainCoordinates function is called after we have loaded the bitmap height map
// into the height map array. Since we only read in the height as the Y coordinate
// we still need to fill out the X and Z coordinates using the for loop. Once that is done
// we also need to move the Z coordinate into the positive range. And finally we scale the
// height of the height map by the heightScale_ value. The height scale is set in the
// terrain setup text file
void TerrainClass::SetTerrainCoordinates()
{
	Log::Debug(THIS_FUNC_EMPTY);

	UINT index = 0;   // position index in the height map

	// loop throught all the elements in the height map array and adjust their coordinates correctly
	for (size_t j = 0; j < terrainHeight_; j++)
	{
		for (size_t i = 0; i < terrainWidth_; i++)
		{
			index = static_cast<UINT>((terrainWidth_ * j) + i);

			// set the X and Z coordinates
			pHeightMap_[index].position.x = static_cast<float>(i);
			pHeightMap_[index].position.z = -static_cast<float>(j);

			// move the terrain depth into the posivite range. For example from (0, -256) to (256, 0);
			pHeightMap_[index].position.z += static_cast<float>(terrainHeight_ - 1);

			// scale the height
			pHeightMap_[index].position.y /= heightScale_;
		}
	}

	return;
}


// this function is used to calculate normals from the height map. And we don't just 
// calculate regular normals per triangle, instead we calculate a weighted normal for
// each vertex based on the average of all the neighbour vertex normals. The end result is 
// shared normals that produce a smooth transition of light over the face of each triangle.
bool TerrainClass::CalculateNormals()
{
	Log::Debug(THIS_FUNC_EMPTY);

	UINT index = 0;       // index for the normals array

	float sum[3];
	float length = 0.0f;
	XMFLOAT3* pNormals = nullptr;

	// create a temporary array to hold the face normal vectors
	pNormals = new XMFLOAT3[(terrainHeight_ - 1) * (terrainWidth_ - 1)];
	COM_ERROR_IF_FALSE(pNormals, "can't allocate memory for the face normal vectors");

	// calculate normals for each terrain face
	this->CalculateFacesNormals(pNormals); 

	// now go through all the vertices and take a sum of the face normals that touch this vertex
	for (int j = 0; j < (int)terrainHeight_; j++)
	{
		for (int i = 0; i < (int)terrainWidth_; i++)
		{
			// initialize the sum
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (terrainWidth_ - 1)) + (i - 1);

				sum[0] += pNormals[index].x;
				sum[1] += pNormals[index].y;
				sum[2] += pNormals[index].z;
			}

			// Bottom right face.
			if ((i<(static_cast<int>(terrainWidth_) - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (terrainWidth_ - 1)) + i;

				sum[0] += pNormals[index].x;
				sum[1] += pNormals[index].y;
				sum[2] += pNormals[index].z;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j<(static_cast<int>(terrainHeight_) - 1)))
			{
				index = (j * (terrainWidth_ - 1)) + (i - 1);

				sum[0] += pNormals[index].x;
				sum[1] += pNormals[index].y;
				sum[2] += pNormals[index].z;
			}

			// Upper right face.
			if ((i < (static_cast<int>(terrainWidth_) - 1)) && (j < (static_cast<int>(terrainHeight_) - 1)))
			{
				index = (j * (terrainHeight_ - 1)) + i;

				sum[0] += pNormals[index].x;
				sum[1] += pNormals[index].y;
				sum[2] += pNormals[index].z;
			}

			// calculate the length of this normal
			length = static_cast<float>(sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2])));

			// get an index to the vertex location in the height map array
			index = (j * terrainWidth_) + i;

			// normalize the final shared normal for this vertex and store it in the height map array
			pHeightMap_[index].normal.x = (sum[0] / length);
			pHeightMap_[index].normal.y = (sum[1] / length);
			pHeightMap_[index].normal.z = (sum[2] / length);
		}
	}

	// release the temporary normals
	_DELETE_ARR(pNormals);

	return true;
}


void TerrainClass::CalculateFacesNormals(DirectX::XMFLOAT3* pNormals)
{
	size_t index = 0;
	size_t index1 = 0;
	size_t index2 = 0;
	size_t index3 = 0;

	float fVector1[3];
	float fVector2[3];

	DirectX::XMFLOAT3 vertex1{};
	DirectX::XMFLOAT3 vertex2{};
	DirectX::XMFLOAT3 vertex3{};
	DirectX::XMVECTOR vector1{};
	DirectX::XMVECTOR vector2{};
	DirectX::XMVECTOR crossProduct{};

	// go through all the faces in the mesh and calculate their normals
	for (size_t j = 0; j < terrainHeight_ - 1; j++)
	{
		for (size_t i = 0; i < terrainWidth_ - 1; i++)
		{
			index1 = ((j + 1) * terrainWidth_) + i;        // bottom left vertex
			index2 = ((j + 1) * terrainWidth_) + (i + 1);  // bottom right vertex
			index3 = (j * terrainWidth_) + i;              // upper left vertex

			// get three vertices from the face
			vertex1 = pHeightMap_[index1].position;
			vertex2 = pHeightMap_[index2].position;
			vertex3 = pHeightMap_[index3].position;

			// calculate the two vectors for this face
			fVector1[0] = vertex1.x - vertex3.x;
			fVector1[1] = vertex1.y - vertex3.y;
			fVector1[2] = vertex1.z - vertex3.z;

			fVector2[0] = vertex3.x - vertex2.x;
			fVector2[1] = vertex3.y - vertex2.y;
			fVector2[2] = vertex3.z - vertex2.z;

			// store float vectors as XMVECTOR
			vector1 = XMVectorSet(fVector1[0], fVector1[1], fVector1[2], 1.0f);
			vector2 = XMVectorSet(fVector2[0], fVector2[1], fVector2[2], 1.0f);

			// calculate an index to the normals array
			index = (j * (terrainWidth_ - 1)) + i;

			// calculate the cross product of those two vectors to get the un-normalized value for this face normal
			crossProduct = XMVector3Cross(vector1, vector2);
			crossProduct = XMVector3Normalize(crossProduct);
			XMStoreFloat3(&pNormals[index], crossProduct);

			//pNormals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			//pNormals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			//pNormals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

			// calculate the length
			//length = static_cast<float>(sqrt(pNormals[index].x * pNormals[index].x) +
			//	(pNormals[index].y * pNormals[index].y) +
			//	(pNormals[index].z * pNormals[index].z));

			// normalize the final value for this face using the length
			//pNormals[index].x = (pNormals[index].x / length);
			//pNormals[index].y = (pNormals[index].y / length);
			//pNormals[index].z = (pNormals[index].z / length);

		}
	}
}


// the function for loading the color map into the height map array;
// it opens a bitmap file and loads in the RGB colour component into the
// height map structure array
bool TerrainClass::LoadColorMap()
{
	errno_t error = 0;
	UINT imageSize = 0;
	FILE* filePtr = nullptr;
	size_t count = 0;
	size_t index = 0;          // a height map position index
	size_t imgBfPos = 0;       // initialize the position in the image data buffer
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	UCHAR* pBitmapImage;

	// open the colour map file in binary
	error = fopen_s(&filePtr, colorMapFilename_, "rb");
	COM_ERROR_IF_FALSE(error == 0, "can't open the colour map file in binary");

	// read in the file header
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	COM_ERROR_IF_FALSE(count == 1, "can't read in the file header");

	// read in the bitmap info header
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	COM_ERROR_IF_FALSE(count == 1, "can't read in the bitmap info header");

	// make sure the colour map dimensions are the same as the terrain
	// dimensions for easy 1 to 1 mapping
	if ((bitmapInfoHeader.biWidth != terrainWidth_) || (bitmapInfoHeader.biHeight != terrainHeight_))
		COM_ERROR_IF_FALSE(false, "the colour map dimensions are no the same as the terrain dimensions");

	// calculate the size of the bitmap image data. Since this is non divide by 2 
	// dimensions (eg. 257x257) we need to add extra byte to each line 
	imageSize = terrainHeight_ * ((terrainWidth_ * 3) + 1);

	// allocate memory for the bitmap image data
	pBitmapImage = new UCHAR[imageSize]{ '\0' };
	COM_ERROR_IF_FALSE(pBitmapImage, "can't allocate memory for the bitmap image data");

	// move to the beginning of the bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// read in the bitmap image data
	count = fread(pBitmapImage, 1, imageSize, filePtr);
	COM_ERROR_IF_FALSE(count == imageSize, "can't read in the bitmap image data");

	// close the file
	error = fclose(filePtr);
	COM_ERROR_IF_FALSE(error == 0, "can't close the bitmap file");

	// read the image data into the colour map portion of the height map structure
	for (size_t j = 0; j < terrainHeight_; j++)
	{
		for (size_t i = 0; i < terrainWidth_; i++)
		{
			// bitmaps are upside down so load bottom to top into the array
			index = (terrainWidth_ * (terrainHeight_ - 1 - j)) + i;

			pHeightMap_[index].color.b = static_cast<float>(pBitmapImage[imgBfPos]) / 255.0f;
			pHeightMap_[index].color.g = static_cast<float>(pBitmapImage[imgBfPos + 1]) / 255.0f;
			pHeightMap_[index].color.r = static_cast<float>(pBitmapImage[imgBfPos + 2]) / 255.f;

			imgBfPos += 3;
		}

		// compensate for extra byte at the end of each line in non-divide by 2 bitmaps (eg. 257x257)
		imgBfPos++;
	}

	// release the bitmap image data
	_DELETE_ARR(pBitmapImage);

	// release the colour map filename now that is has been read in
	_DELETE(colorMapFilename_);

	return true;
}


// BuildTerrainModel is the function that takes the points in the height map array and
// creates a 3D polygon mesh from them. It loops through the height map array and
// grabs four points at a time and creates two triangles from those four points.
// The final 3D terrain model is stored in the pVerticesData_ array.
bool TerrainClass::BuildTerrainModel()
{
	Log::Debug(THIS_FUNC_EMPTY);

	UINT index = 0;   // initialize the index into the height map array
	size_t index1 = 0;
	size_t index2 = 0;
	size_t index3 = 0;
	size_t index4 = 0;
	VERTEX* pVerticesArray = nullptr;
	UINT* pIndicesArray = nullptr;
	VERTEX vertex{};  // temporal vertex for writing data into the vertices array

	// calculate the number of vertices in the 3D terrain model
	SetVertexCount((terrainHeight_ - 1) * (terrainWidth_ - 1) * 6);
	SetIndexCount(GetVertexCount());         // we have the same indices count as the vertices count

	// allocate memory for the vertex and index array
	AllocateVerticesAndIndicesArrays(GetVertexCount(), GetIndexCount());

	// we use the direct pointers to vertex/index array for better performance during initialization it with data
	pVerticesArray = *(GetAddressOfVerticesData());
	pIndicesArray = *(GetAddressOfIndicesData());

	// load the 3D terrain model width the height map terrain data;
	// we will be creating 2 triangles for each of the four points in a quad
	for (size_t j = 0; j < (terrainHeight_ - 1); j++)
	{
		for (size_t i = 0; i < (terrainWidth_ - 1); i++)
		{
			// get the indices to the four points of the quad
			index1 = (terrainWidth_ * j) + i;         // upper left
			index2 = (terrainWidth_ * j) + (i+1);     // upper right
			index3 = (terrainWidth_ * (j+1)) + i;     // bottom left
			index4 = (terrainWidth_ * (j+1)) + (i+1); // bottom right

			// now create two triangles for that quad
			// triangle 1 - upper left
			pVerticesArray[index].position = pHeightMap_[index1].position;
			pVerticesArray[index].texture.x = 0.0f;
			pVerticesArray[index].texture.y = 0.0f;
			pVerticesArray[index].normal = pHeightMap_[index1].normal;
			pVerticesArray[index].color.x = pHeightMap_[index1].color.r;
			pVerticesArray[index].color.y = pHeightMap_[index1].color.g;
			pVerticesArray[index].color.z = pHeightMap_[index1].color.b;
			pIndicesArray[index] = index;
			index++;

			// triangle 1 - upper right
			pVerticesArray[index].position = pHeightMap_[index2].position;
			pVerticesArray[index].texture.x = 1.0f;
			pVerticesArray[index].texture.y = 0.0f;
			pVerticesArray[index].normal = pHeightMap_[index2].normal;
			pVerticesArray[index].color.x = pHeightMap_[index2].color.r;
			pVerticesArray[index].color.y = pHeightMap_[index2].color.g;
			pVerticesArray[index].color.z = pHeightMap_[index2].color.b;
			pIndicesArray[index] = index;
			index++;

			// triangle 1 - bottom left
			pVerticesArray[index].position = pHeightMap_[index3].position;
			pVerticesArray[index].texture.x = 0.0f;
			pVerticesArray[index].texture.y = 1.0f;
			pVerticesArray[index].normal = pHeightMap_[index3].normal;
			pVerticesArray[index].color.x = pHeightMap_[index3].color.r;
			pVerticesArray[index].color.y = pHeightMap_[index3].color.g;
			pVerticesArray[index].color.z = pHeightMap_[index3].color.b;
			pIndicesArray[index] = index;
			index++;



			// triangle 2 - bottom left
			pVerticesArray[index].position = pHeightMap_[index3].position;
			pVerticesArray[index].texture.x = 0.0f;
			pVerticesArray[index].texture.y = 1.0f;
			pVerticesArray[index].normal = pHeightMap_[index3].normal;
			pVerticesArray[index].color.x = pHeightMap_[index3].color.r;
			pVerticesArray[index].color.y = pHeightMap_[index3].color.g;
			pVerticesArray[index].color.z = pHeightMap_[index3].color.b;
			pIndicesArray[index] = index;
			index++;

			// triangle 2 - upper right
			pVerticesArray[index].position = pHeightMap_[index2].position;
			pVerticesArray[index].texture.x = 1.0f;
			pVerticesArray[index].texture.y = 0.0f;
			pVerticesArray[index].normal = pHeightMap_[index2].normal;
			pVerticesArray[index].color.x = pHeightMap_[index2].color.r;
			pVerticesArray[index].color.y = pHeightMap_[index2].color.g;
			pVerticesArray[index].color.z = pHeightMap_[index2].color.b;
			pIndicesArray[index] = index;
			index++;

			// triangle 2 - bottom right
			pVerticesArray[index].position = pHeightMap_[index4].position;
			pVerticesArray[index].texture.x = 1.0f;
			pVerticesArray[index].texture.y = 1.0f;
			pVerticesArray[index].normal = pHeightMap_[index4].normal;
			pVerticesArray[index].color.x = pHeightMap_[index4].color.r;
			pVerticesArray[index].color.y = pHeightMap_[index4].color.g;
			pVerticesArray[index].color.z = pHeightMap_[index4].color.b;
			pIndicesArray[index] = index;
			index++;
		}
	}
	
	return true;
}


// The CalculateTerrainVectors() function is used for traversing through the terrain model
// and finding three vertices for each triangle. Then it takes those three vertices
// and passes them into the CalculateTangentBinormal function to calculate the tangent
// and binormal for that triangle. The tangent and binormal are passed back from the 
// function by reference and then we copy them into the terrain model
void TerrainClass::CalculateTerrainVectors()
{
	bool calcNormalsForTerrain = false;
	std::unique_ptr<ModelMath> pModelMath = std::make_unique<ModelMath>(); // for calculations of the the terrain tangent, binormal, etc.

	// calculate tangent and binormal for the terrain
	pModelMath->CalculateModelVectors(GetVerticesData(), this->GetVertexCount(), calcNormalsForTerrain);
	return;
}


// LoadTerrainCells is the function where we create the array of terrain cell objects.
// We specify the size of the cell (for example: 33x33) and then create the array. 
// Once the array is created we loop through it and initialize each cell with its part of 
// the terrain model. We send a pointer to the terrain model and indices (i and j)
// for the current position of the cell so that it knows where to read in the data from
// the terrain model to build the current cell
bool TerrainClass::LoadTerrainCells(ID3D11Device* pDevice)
{
	UINT cellHeight = 33;    // set the height and width of each terrain cell to a fixed 33x33 vertex array
	UINT cellWidth = 33;
	UINT cellRowCount = 0;
	UINT index = 0;
	bool result = false;

	// calculate the number of cells needed to store the terrain data
	cellRowCount = (terrainWidth_ - 1) / (cellWidth - 1);
	cellCount_ = cellRowCount * cellRowCount;

	try
	{
		// create the terrain cells array
		pTerrainCells_ = new TerrainCellClass[cellCount_];
	}
	catch(std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(pTerrainCells_, "can't allocate memory for the terrain cell class object");
	}

	// loop through and initialize all the terrain cells
	for (UINT j = 0; j < cellRowCount; j++)
	{
		for (UINT i = 0; i < cellRowCount; i++)
		{
			index = (cellRowCount * j) + i;

			result = pTerrainCells_[index].Initialize(pDevice, GetVerticesData(), i, j, cellHeight, cellWidth, terrainWidth_);
			COM_ERROR_IF_FALSE(result, "can't initialize a terrain cell model");
		}
	}
	
	return true;
}


void TerrainClass::SkipUntilSymbol(ifstream & fin, char symbol)
{
	char input = ' ';

	fin.get(input);
	while (input != symbol)
	{
		fin.get(input);
	}
}


// the following function are used to render the individual terrain cells as well as 
// the orange bounding boxes around each cell. Each of the render functions takes
// as input the cell ID so it knows which cell to render or which cell to get the 
// index count from.
bool TerrainClass::Render(ID3D11DeviceContext* pDeviceContext, UINT cellID)
{
	pTerrainCells_[cellID].RenderCell(pDeviceContext);
	return true;
}

void TerrainClass::RenderCellLines(ID3D11DeviceContext* pDeviceContext, UINT cellID)
{
	pTerrainCells_[cellID].RenderLineBuffers(pDeviceContext);
	return;
}

UINT TerrainClass::GetCellIndexCount(UINT cellID) const
{
	return pTerrainCells_[cellID].GetTerrainCellIndexCount();
}

UINT TerrainClass::GetCellLinesIndexCount(UINT cellID) const
{
	return pTerrainCells_[cellID].GetCellLinesIndexCount();
}

UINT TerrainClass::GetCellCount() const
{
	return cellCount_;
}


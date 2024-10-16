#include "TerrainInitializer.h"
#include "../Engine/log.h"



///////////////////////////////////////////////////////////

void SkipUntilSymbol(std::ifstream & fin, char symbol)
{
	char input = ' ';

	fin.get(input);
	while (input != symbol)
	{
		fin.get(input);
	}
}





bool TerrainInitializer::LoadSetupFile(const std::string & setupFilePath)
{
	// LoadSetupFile() is a function that takes in the terrain setup file and stores
	// all the values so that we can construct the terrain based on what is in that file.
	// Reads the bitmap height map file, the terrain width and height, the terrain
	// height scaling value, etc.

	Log::Debug("LoadSetupFile");

	// check input params
	assert(!setupFilePath.empty());


	// set minimal possible params for the terrain
	const UINT minTerrainDimensionMagnitude = 1;
	const float minHeightScale = 0.0f;

	std::ifstream fin;

	// open the setup file. If it could not open the file then exit
	fin.open(setupFilePath);
	if (fin.fail())
	{
		throw EngineException("can't open the setup file");
	}

	SkipUntilSymbol(fin, ':'); // read up to the terrain filename
	fin >> setupData_.terrainFilename;   // read in the terrain file name


	SkipUntilSymbol(fin, ':'); // read up to the value of terrain depth
	fin >> setupData_.terrainDepth;     // read in the terrain depth (Z-coord)


	SkipUntilSymbol(fin, ':'); // read up to the value of terrain width
	fin >> setupData_.terrainWidth;      // read in the terrain width (X-coord)


	SkipUntilSymbol(fin, ':'); // read up to the value of the terrain height scaling
	fin >> setupData_.heightScale;       // read in the terrain height scaling

	SkipUntilSymbol(fin, ':'); // read up to the value of the color map bitmap file name
	fin >> setupData_.colorMapFilename;


	// make confidence that we have got proper terrain values
	assert(setupData_.terrainDepth > minTerrainDimensionMagnitude);
	assert(setupData_.terrainWidth > minTerrainDimensionMagnitude);
	assert(setupData_.heightScale > minHeightScale);
	assert(!setupData_.terrainFilename.empty());
	assert(!setupData_.colorMapFilename.empty());


	// close the setup file
	fin.close();

	return true;
}


#if 0


bool TerrainInitializer::Initialize(Settings* pSettings,
	std::shared_ptr<TerrainSetupData> pTerrainSetupData,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	std::vector<RenderableGameObject*> & terrainCellsArr,
	ModelInitializerInterface* pModelInitializer,
	ModelToShaderMediatorInterface* pModelToShaderMediator)
{
	Log::Debug();

	// check input params
	assert(pSettings != nullptr);
	assert(pDevice != nullptr);
	assert(pDeviceContext != nullptr);
	assert(pModelInitializer != nullptr);
	assert(pModelToShaderMediator != nullptr);

	std::string setupFilename{ "" };
	bool result = false;

	try
	{
		// make a local copy of ptr to the terrain setup data structure
		pSetupData_ = pTerrainSetupData;


	

		


		// get the terrain filename, dimensions, and so forth from the setup file
		result = this->LoadSetupFile(setupFilename, pTerrainSetupData);
		Assert::True(result, "can't load the setup file");


		if (loadRawHeightMap)
		{
			// initialize the terrain height map with the data from the raw file
			LoadRawHeightMap();
		}
		else
		{
			// initialize the terrain height map with the data from the bitmap file
			LoadBitmapHeightMap();
		}


		////////////////////////////////////////////////////////


		// setup the X and Z coordinates for the height map as well as scale the terrain
		// height by the height scale value
		SetupTerrainCoordinates();

		// calculate the normals for the terrain data
		result = CalculateNormals();
		Assert::True(result, "can't calculate the normals for the terrain");

		// load in the colour map for the terrain
		LoadColorMap();
		Assert::True(result, "can't load in the colour map");

		// now build the 3D model of the terrain
		result = BuildTerrainModel();
		Assert::True(result, "can't build a terrain model");


		// calculate the tangent and binormal for the terrain model
		ComputeTerrainVectors();

		// create and load cells with the terrain data
		result = LoadTerrainCells(pDevice, 
			pDeviceContext, 
			terrainCellsArr,
			pModelInitializer, 
			pModelToShaderMediator);
		Assert::True(result, "can't load terrain cells");

		// after initialization of the terrain we have to clear memory
		Shutdown();

	}
	catch (EngineException & e)
	{
		// in case of any error we have to clear memory
		Shutdown();

		Log::Error(e, false);
		Log::Error("can't initialize the terrain model");
		return false;
	}


	return true;
}

///////////////////////////////////////////////////////////

void TerrainInitializer::Shutdown()
{
	// clear memory after initialization of the terrain

	// we can now release the height map since it is no longer seeded in memory once
	// the 3D terrain model has been built
	heightMapArr_.clear();

	// clear the vertices/indices arrays as well
	verticesArr_.clear();
	indicesArr_.clear();
}

///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////

void TerrainInitializer::LoadBitmapHeightMap()
{
	// LoadBitmapHeightMap() loads the bitmap file containing the height map into the 
	// height map array. Note that the bitmap format contains red, green, and blue colours.
	// But since this being treated like a grey scale image you can read either the red, green,
	// or blue colour as the will all be the same grey value and you only need one of them.
	// Also note that we use odd width_x_height of the bitmap because we need an odd number of 
	// points to build an even number of quads. And finally the bitmap format stores the
	// image upside down. And because of this we first need to read the data into an array,
	// and then copy that array into the height map from the bottom up.


	Log::Debug();

	std::vector<UCHAR> bitmapImageData;    // an array for the bitmap image data
	FILE* filePtr = nullptr;
	errno_t error = 0;
	UINT imageSize = 0;

	size_t count = 0;
	size_t index = 0;                   // a height map position index
	size_t imgBufferPos = 0;            // the position in the image data buffer
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	UCHAR height = 0;

	////////////////////////////////////////////////

	// start by resizing the array structure to hold the height map data
	try
	{
		UINT terrainArea = pSetupData_->terrainWidth * pSetupData_->terrainHeight;
		heightMapArr_.resize(terrainArea);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(e.what());
		throw EngineException("can't allocate memory for a height map array");
	}

	////////////////////////////////////////////////

	// open the bitmap map file in binary
	error = fopen_s(&filePtr, pSetupData_->terrainFilename.c_str(), "rb");
	Assert::True(error == 0, "can't open the bitmap map file");

	// read in the bitmap file header
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	Assert::True(count == 1, "can't read in the bitmap file header");

	// read in the bitmap info header
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	Assert::True(count == 1, "can't read in the bitmap info header");

	// make sure the height map dimensions are the same as the terrain dimensions
	// for easy 1 to 1 mapping
	if ((bitmapInfoHeader.biHeight != pSetupData_->terrainHeight) || 
		(bitmapInfoHeader.biWidth != pSetupData_->terrainWidth))
		throw EngineException("map dimensions are not the same as the terrain dimensions");

	////////////////////////////////////////////////

	// calculate the size of the bitmap image data;
	// since we use non-divide by 2 dimensions (eg. 257x257) we need to add 
	// an extra byte to each line.
	imageSize = pSetupData_->terrainHeight * ((pSetupData_->terrainWidth * 3) + 1);

	// allocate memory for the bitmap image data
	try
	{
		bitmapImageData.resize(imageSize);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(e.what());
		throw EngineException("can't allocate memory for the bitmap image data");
	}

	////////////////////////////////////////////////

	// move to the beginning of the bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// read in the bitmap image data
	count = fread(bitmapImageData.data(), 1, imageSize, filePtr);
	Assert::True(count == imageSize, "can't read in the bitmap image data");

	// close the file
	error = fclose(filePtr);
	Assert::True(error == 0, "can't close the file");

	////////////////////////////////////////////////

	// read the image data into the height map array
	for (size_t j = 0; j < pSetupData_->terrainHeight; j++)
	{
		for (size_t i = 0; i < pSetupData_->terrainWidth; i++)
		{
			// bitmaps are upside down so load bottom to top into the height map array
			index = (pSetupData_->terrainWidth * (pSetupData_->terrainHeight - 1 - j)) + i;

			// get the grey scale pixel value from the bitmap image data at this location
			height = bitmapImageData[imgBufferPos];

			// store the pixel value as the height at this point in the height map array
			heightMapArr_[index].position.y = static_cast<float>(height);

			// increment the bitmap image data index
			imgBufferPos += 3;
		}

		// compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257)
		imgBufferPos++;
	}

	return;

} // end LoadBitmapHeightMap

///////////////////////////////////////////////////////////

bool TerrainInitializer::LoadRawHeightMap()
{
	// The LoadRawHeightMap() loads 16 bit RAW height map files. It works in the exact same 
	// fashion as LoadBitmapHeightMap() but handles the RAW format instead. Since this is a 
	// 16 bit format we use unsigned short instead of unsigned char to create the array that
	// the data will be read into. Also when we parse through the array to copy the data
	// into the height map structure we don't have to traverse it backwards because the RAW
	// format is not stored unpside down like bitmaps.

	Log::Debug();

	errno_t error = 0;
	UINT index = 0;
	FILE* pFile = nullptr;
	ULONGLONG imageSize = 0;
	ULONGLONG count = 0;

	try
	{
		// allocate memory for an array to hold the height map data
		UINT terrainArea = pSetupData_->terrainWidth * pSetupData_->terrainHeight;
		heightMapArr_.resize(terrainArea);

		// open the 16 bit raw height map file for reading in binary
		error = fopen_s(&pFile, pSetupData_->terrainFilename.c_str(), "rb");
		Assert::True(error == 0, "can't open the 16 bit raw height map file");

		// calculate the size of the raw image data
		imageSize = terrainArea;

		// allocate memory for the raw image data
		std::vector<USHORT> rawImageData(imageSize);

		// read in the raw image data
		count = fread(rawImageData.data(), sizeof(USHORT), imageSize, pFile);
		Assert::True(count == imageSize, "can't read in the raw image data");

		// close the file
		error = fclose(pFile);
		Assert::True(error == 0, "can't close the file");

		////////////////////////////////////////////////

		// copy the image data into the height map array
		for (UINT j = 0; j < pSetupData_->terrainHeight; j++)
		{
			for (UINT i = 0; i < pSetupData_->terrainWidth; i++)
			{
				index = (pSetupData_->terrainWidth * j) + i;

				// store the height at this point in the height map array
				this->heightMapArr_[index].position.y = static_cast<float>(rawImageData[index]);
			}
		}
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(e.what());
		Log::Error("cant allocate memory for a height map array / raw image data");
	}
	catch (EngineException & e)
	{
		Log::Error(e);
		return false;
	}

	return true;

} // end LoadRawHeightMap

///////////////////////////////////////////////////////////

void TerrainInitializer::SetupTerrainCoordinates()
{
	// The SetupTerrainCoordinates function is called after we have loaded the bitmap height map
	// into the height map array. Since we only read in the height as the Y coordinate
	// we still need to fill out the X and Z coordinates using the for loop. Once that is done
	// we also need to move the Z coordinate into the positive range. And finally we scale the
	// height of the height map by the heightScale_ value. The height scale is set in the
	// terrain setup text file

	Log::Debug();

	// position index in the height map
	UINT index = 0;   

	// loop throught all the elements in the height map array and adjust their coordinates correctly
	for (UINT j = 0; j < pSetupData_->terrainHeight; j++)
	{
		for (UINT i = 0; i < pSetupData_->terrainWidth; i++)
		{
			index = (pSetupData_->terrainWidth * j) + i;

			// set the X and Z coordinates
			heightMapArr_[index].position.x = static_cast<float>(i);
			heightMapArr_[index].position.z = -static_cast<float>(j);

			// move the terrain depth into the posivite range. For example from (0, -256) to (256, 0);
			heightMapArr_[index].position.z += static_cast<float>(pSetupData_->terrainHeight - 1);

			// scale the height
			heightMapArr_[index].position.y /= pSetupData_->heightScale;
		}
	}

	return;

} // end SetupTerrainCoordinates

///////////////////////////////////////////////////////////

bool TerrainInitializer::CalculateNormals()
{
	// this function is used to calculate normals from the height map. And we don't just 
	// calculate regular normals per triangle, instead we calculate a weighted normal for
	// each vertex based on the average of all the neighbour vertex normals. The end result is 
	// shared normals that produce a smooth transition of light over the face of each triangle.

	Log::Debug();



	float sum[3]{ 0.0f };     
	float length_inv = 0.0f;   // inverted length of a weighted normal

	UINT index = 0;       // index for the normals array
	UINT terrainNormalsCount = (pSetupData_->terrainHeight - 1) * (pSetupData_->terrainWidth - 1);

	int terrainWidth = static_cast<int>(pSetupData_->terrainWidth);
	int terrainHeight = static_cast<int>(pSetupData_->terrainHeight);
	
	// create a temporary array to hold the faces normal vectors
	std::vector<DirectX::XMFLOAT3> normalsArr(terrainNormalsCount);

	/////////////////////////////////////////////

	// calculate normals for each terrain face
	this->CalculateFacesNormals(normalsArr);

	// now go through all the vertices and take a sum of the face normals that touch this vertex
	for (int j = 0; j < terrainHeight; j++)
	{
		for (int i = 0; i < terrainWidth; i++)
		{
			// reset the sum
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (terrainWidth - 1)) + (i - 1);

				sum[0] += normalsArr[index].x;
				sum[1] += normalsArr[index].y;
				sum[2] += normalsArr[index].z;
			}

			// Bottom right face.
			if ((i<(terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (terrainWidth - 1)) + i;

				sum[0] += normalsArr[index].x;
				sum[1] += normalsArr[index].y;
				sum[2] += normalsArr[index].z;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j<(terrainHeight - 1)))
			{
				index = (j * (terrainWidth - 1)) + (i - 1);

				sum[0] += normalsArr[index].x;
				sum[1] += normalsArr[index].y;
				sum[2] += normalsArr[index].z;
			}

			// Upper right face.
			if ((i < (terrainWidth - 1)) && (j < (terrainHeight - 1)))
			{
				index = (j * (terrainWidth - 1)) + i;

				sum[0] += normalsArr[index].x;
				sum[1] += normalsArr[index].y;
				sum[2] += normalsArr[index].z;
			}

			/////////////////////////////////////////////

			// calculate the length of this normal
			length_inv = 1.0f / (sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2])));
			
			// get an index to the vertex location in the height map array
			index = (j * pSetupData_->terrainWidth) + i;

			// normalize the final shared normal for this vertex and store it in the height map array
			heightMapArr_[index].normal.x = (sum[0] * length_inv);
			heightMapArr_[index].normal.y = (sum[1] * length_inv);
			heightMapArr_[index].normal.z = (sum[2] * length_inv);
		}
	}

	return true;

} // end CalculateNormals

///////////////////////////////////////////////////////////

void TerrainInitializer::CalculateFacesNormals(std::vector<DirectX::XMFLOAT3> & normalsArr)
{
	DirectX::XMFLOAT3 vertex1;
	DirectX::XMFLOAT3 vertex2;
	DirectX::XMFLOAT3 vertex3;
	DirectX::XMVECTOR vector1;
	DirectX::XMVECTOR vector2;
	DirectX::XMVECTOR crossProduct;

	float fVector1[3]{ 0.0f };
	float fVector2[3]{ 0.0f };

	size_t index = 0;
	size_t index1 = 0;
	size_t index2 = 0;
	size_t index3 = 0;

	

	// go through all the faces in the mesh and calculate their normals
	for (size_t j = 0; j < pSetupData_->terrainHeight - 1; j++)
	{
		for (size_t i = 0; i < pSetupData_->terrainWidth - 1; i++)
		{
			index1 = ((j + 1) * pSetupData_->terrainWidth) + i;        // bottom left vertex
			index2 = ((j + 1) * pSetupData_->terrainWidth) + (i + 1);  // bottom right vertex
			index3 = (j * pSetupData_->terrainWidth) + i;              // upper left vertex

			// get three vertices from the face
			vertex1 = heightMapArr_[index1].position;
			vertex2 = heightMapArr_[index2].position;
			vertex3 = heightMapArr_[index3].position;

			// calculate the two vectors for this face
			fVector1[0] = vertex1.x - vertex3.x;
			fVector1[1] = vertex1.y - vertex3.y;
			fVector1[2] = vertex1.z - vertex3.z;

			fVector2[0] = vertex3.x - vertex2.x;
			fVector2[1] = vertex3.y - vertex2.y;
			fVector2[2] = vertex3.z - vertex2.z;

			// store float vectors as XMVECTOR
			vector1 = DirectX::XMVectorSet(fVector1[0], fVector1[1], fVector1[2], 1.0f);
			vector2 = DirectX::XMVectorSet(fVector2[0], fVector2[1], fVector2[2], 1.0f);

			// calculate an index to the normals array
			index = (j * (pSetupData_->terrainWidth - 1)) + i;

			// calculate the cross product of those two vectors to get the un-normalized value for this face normal
			crossProduct = DirectX::XMVector3Cross(vector1, vector2);

			// normalize the final value for this face and store it into the normalsArr
			crossProduct = DirectX::XMVector3Normalize(crossProduct);
			DirectX::XMStoreFloat3(&(normalsArr[index]), crossProduct);
		}
	}
}

///////////////////////////////////////////////////////////

void TerrainInitializer::LoadColorMap()
{
	// the function for loading the color map into the height map array;
	// it opens a bitmap file and loads in the RGB colour component into the
	// height map structure array

	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	
	size_t count = 0;
	size_t index = 0;          // a height map position index
	size_t imgBfPos = 0;       // initialize the position in the image data buffer
	errno_t error = 0;
	UINT imageSize = 0;
	FILE* filePtr = nullptr;
	
	std::vector<UCHAR> bitmapImageData;    // an array for the bitmap image data
	

	////////////////////////////////////////////////

	// open the colour map file in binary
	error = fopen_s(&filePtr, pSetupData_->colorMapFilename.c_str(), "rb");
	Assert::True(error == 0, "can't open the colour map file in binary");

	// read in the file header
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	Assert::True(count == 1, "can't read in the file header");

	// read in the bitmap info header
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	Assert::True(count == 1, "can't read in the bitmap info header");

	// make sure the colour map dimensions are the same as the terrain
	// dimensions for easy 1 to 1 mapping
	if ((bitmapInfoHeader.biWidth != pSetupData_->terrainWidth) ||
		(bitmapInfoHeader.biHeight != pSetupData_->terrainHeight))
		throw EngineException("the colour map dimensions are not the same as the terrain dimensions");

	// calculate the size of the bitmap image data. Since this is non divide by 2 
	// dimensions (eg. 257x257) we need to add extra byte to each line 
	imageSize = pSetupData_->terrainHeight * ((pSetupData_->terrainWidth * 3) + 1);

	////////////////////////////////////////////////

	try
	{
		// allocate memory for the bitmap image data
		bitmapImageData.resize(imageSize);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(e.what());
		throw EngineException("can't resize the bitmap image data array");
	}


	// move to the beginning of the bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// read in the bitmap image data
	count = fread(bitmapImageData.data(), 1, imageSize, filePtr);
	Assert::True(count == imageSize, "can't read in the bitmap image data");

	// close the file
	error = fclose(filePtr);
	Assert::True(error == 0, "can't close the bitmap file");

	float inv_255 = 1.0f / 255.0f;

	// read the image data into the colour map portion of the height map structure
	for (size_t j = 0; j < pSetupData_->terrainHeight; j++)
	{
		for (size_t i = 0; i < pSetupData_->terrainWidth; i++)
		{
			// bitmaps are upside down so load bottom to top into the array
			index = (pSetupData_->terrainWidth * (pSetupData_->terrainHeight - 1 - j)) + i ;

			// pay attention that we record colour values in the revers order (not RGB but BGR)
			heightMapArr_[index].color.z = (float)(bitmapImageData[imgBfPos] * inv_255);
			heightMapArr_[index].color.y = (float)(bitmapImageData[imgBfPos + 1] * inv_255);
			heightMapArr_[index].color.x = (float)(bitmapImageData[imgBfPos + 2] * inv_255);

			imgBfPos += 3;
		}

		// compensate for extra byte at the end of each line in non-divide by 2 bitmaps (eg. 257x257)
		imgBfPos++;
	}

	return;

} // end LoadColorMap

///////////////////////////////////////////////////////////

bool TerrainInitializer::BuildTerrainModel()
{
	// BuildTerrainModel is the function that takes the points in the height map array and
	// creates a 3D polygon mesh from them. It loops through the height map array and
	// grabs four points at a time and creates two triangles from those four points.
	// The final 3D terrain model is stored in the pVerticesData_ array.

	Log::Debug();

	UINT index = 0;   // initialize the index into the height map array
	UINT verticesArrIndex = 0;
	UINT indicesArrIndex = 0;
	size_t index1 = 0;
	size_t index2 = 0;
	size_t index3 = 0;
	size_t index4 = 0;

	DirectX::XMFLOAT2 upperLeftTextureCoord  { 0.0f, 0.0f };
	DirectX::XMFLOAT2 upperRightTextureCoord { 1.0f, 0.0f };
	DirectX::XMFLOAT2 bottomLeftTextureCoord { 0.0f, 1.0f };
	DirectX::XMFLOAT2 bottomRightTextureCoord{ 1.0f, 1.0f };


	// calculate the number of vertices in the 3D terrain model:
	// width * height * 4    (because ina quad we have only 4 vertices)
	UINT verticesCount = (pSetupData_->terrainWidth - 1) * (pSetupData_->terrainHeight - 1) * 4;

	// calculate the number of indices in the 3D terrain model:
	// width * height * 6   (because we must have 6 indices to make a quad)
	UINT indicesCount = (pSetupData_->terrainWidth - 1) * (pSetupData_->terrainHeight - 1) * 6;
	verticesCount = indicesCount;
	
	// resize vertices/indices arrays so we can write terrain's data into it
	this->verticesArr_.resize(verticesCount);
	this->indicesArr_.resize(indicesCount);

	// load the 3D terrain model width the height map terrain data;
	// we will be creating 2 triangles for each of the four points in a quad
	for (size_t j = 0; j < (pSetupData_->terrainHeight - 1); j++)
	{
		for (size_t i = 0; i < (pSetupData_->terrainWidth - 1); i++)
		{
			// get the indices to the four points of the quad
			index1 = (pSetupData_->terrainWidth * j) + i;             // upper left / right
			index2 = (pSetupData_->terrainWidth * j) + (i + 1);       
			index3 = (pSetupData_->terrainWidth * (j + 1)) + i;       // bottom left / right
			index4 = (pSetupData_->terrainWidth * (j + 1)) + (i + 1); 

			/////////////////////////////////////////////////////////

			// now create two triangles for that quad
			// triangle 1 - upper left
			verticesArr_[index].position = heightMapArr_[index1].position;
			verticesArr_[index].texture = upperLeftTextureCoord;
			verticesArr_[index].normal = heightMapArr_[index1].normal;
			verticesArr_[index].color = heightMapArr_[index1].color;
			indicesArr_[index] = index;
			index++;

			// triangle 1 - upper right
			verticesArr_[index].position = heightMapArr_[index2].position;
			verticesArr_[index].texture = upperRightTextureCoord;
			verticesArr_[index].normal = heightMapArr_[index2].normal;
			verticesArr_[index].color = heightMapArr_[index2].color;
			indicesArr_[index] = index;
			index++;

			// triangle 1 - bottom left
			verticesArr_[index].position = heightMapArr_[index3].position;
			verticesArr_[index].texture = bottomLeftTextureCoord;
			verticesArr_[index].normal = heightMapArr_[index3].normal;
			verticesArr_[index].color = heightMapArr_[index3].color;
			indicesArr_[index] = index;
			index++;


			/////////////////////////////////////////////////////////


			// triangle 2 - bottom left
			verticesArr_[index] = verticesArr_[index - 1]; // use the same vertex data (bottom left)
			indicesArr_[index] = index - 1;
			index++;

			// triangle 2 - upper right
			verticesArr_[index] = verticesArr_[index - 3]; // use the same vertex data (upper right)
			indicesArr_[index] = index - 3;
			index++;

			// triangle 2 - bottom right
			verticesArr_[index].position = heightMapArr_[index4].position;
			verticesArr_[index].texture = bottomRightTextureCoord;
			verticesArr_[index].normal = heightMapArr_[index4].normal;
			verticesArr_[index].color = heightMapArr_[index4].color;
			indicesArr_[index] = index;
			index++;

		} // for
	} // for

	return true;

} // end BuildTerrainModel


bool TerrainInitializer::BuildTerrainModel_OptimizedVersion()
{
	// BuildTerrainModel is the function that takes the points in the height map array and
	// creates a 3D polygon mesh from them. It loops through the height map array and
	// grabs four points at a time and creates two triangles from those four points.
	// The final 3D terrain model is stored in the pVerticesData_ array.

	Log::Debug();

	UINT index = 0;   // initialize the index into the height map array
	UINT verticesArrIndex = 0;
	UINT indicesArrIndex = 0;
	size_t index1 = 0;
	size_t index2 = 0;
	size_t index3 = 0;
	size_t index4 = 0;

	DirectX::XMFLOAT2 upperLeftTextureCoord{ 0.0f, 0.0f };
	DirectX::XMFLOAT2 upperRightTextureCoord{ 1.0f, 0.0f };
	DirectX::XMFLOAT2 bottomLeftTextureCoord{ 0.0f, 1.0f };
	DirectX::XMFLOAT2 bottomRightTextureCoord{ 1.0f, 1.0f };


	// calculate the number of vertices in the 3D terrain model:
	// width * height * 4    (because ina quad we have only 4 vertices)
	UINT verticesCount = (pSetupData_->terrainWidth - 1) * (pSetupData_->terrainHeight - 1) * 4;

	// calculate the number of indices in the 3D terrain model:
	// width * height * 6   (because we must have 6 indices to make a quad)
	UINT indicesCount = (pSetupData_->terrainWidth - 1) * (pSetupData_->terrainHeight - 1) * 6;

	// resize vertices/indices arrays so we can write terrain's data into it
	this->verticesArr_.resize(verticesCount);
	this->indicesArr_.resize(indicesCount);


	// load the 3D terrain model width the height map terrain data;
	// we will be creating 2 triangles for each of the four points in a quad
	for (size_t j = 0; j < (pSetupData_->terrainHeight - 1); j++)
	{
		for (size_t i = 0; i < (pSetupData_->terrainWidth - 1); i++)
		{
			// get the indices to the four points of the quad
			index1 = (pSetupData_->terrainWidth * j) + i;             // upper left / right
			index2 = (pSetupData_->terrainWidth * j) + (i + 1);
			index3 = (pSetupData_->terrainWidth * (j + 1)) + i;       // bottom left / right
			index4 = (pSetupData_->terrainWidth * (j + 1)) + (i + 1);

			/////////////////////////////////////////////////////////

			// now create two triangles for that quad
			// triangle 1 - upper left
			verticesArr_[verticesArrIndex].position = heightMapArr_[index1].position;
			verticesArr_[verticesArrIndex].texture = upperLeftTextureCoord;
			verticesArr_[verticesArrIndex].normal = heightMapArr_[index1].normal;
			verticesArr_[verticesArrIndex].color = heightMapArr_[index1].color;
			verticesArrIndex++;
			indicesArr_[indicesArrIndex] = index;
			indicesArrIndex++;

			// triangle 1 - upper right
			verticesArr_[verticesArrIndex].position = heightMapArr_[index2].position;
			verticesArr_[verticesArrIndex].texture = upperRightTextureCoord;
			verticesArr_[verticesArrIndex].normal = heightMapArr_[index2].normal;
			verticesArr_[verticesArrIndex].color = heightMapArr_[index2].color;
			verticesArrIndex++;
			indicesArr_[indicesArrIndex] = index;
			indicesArrIndex++;

			// triangle 1 - bottom left
			verticesArr_[verticesArrIndex].position = heightMapArr_[index3].position;
			verticesArr_[verticesArrIndex].texture = bottomLeftTextureCoord;
			verticesArr_[verticesArrIndex].normal = heightMapArr_[index3].normal;
			verticesArr_[verticesArrIndex].color = heightMapArr_[index3].color;
			verticesArrIndex++;
			indicesArr_[indicesArrIndex] = index;
			indicesArrIndex++;


			/////////////////////////////////////////////////////////


			// triangle 2 - bottom left
			verticesArr_[verticesArrIndex] = verticesArr_[verticesArrIndex - 1]; // use the same vertex data (bottom left)
			verticesArrIndex++;
			indicesArr_[indicesArrIndex] = indicesArrIndex - 1;
			indicesArrIndex++;

			// triangle 2 - upper right
			verticesArr_[verticesArrIndex] = verticesArr_[verticesArrIndex - 3]; // use the same vertex data (upper right)
			verticesArrIndex++;
			indicesArr_[indicesArrIndex] = indicesArrIndex - 3;
			indicesArrIndex++;

			// triangle 2 - bottom right
			verticesArr_[verticesArrIndex].position = heightMapArr_[index4].position;
			verticesArr_[verticesArrIndex].texture = bottomRightTextureCoord;
			verticesArr_[verticesArrIndex].normal = heightMapArr_[index4].normal;
			verticesArr_[verticesArrIndex].color = heightMapArr_[index4].color;
			verticesArrIndex++;
			indicesArr_[indicesArrIndex] = indicesArrIndex;
			indicesArrIndex++;

		} // for
	} // for

	return true;

} // end BuildTerrainModelOptimized

///////////////////////////////////////////////////////////

void TerrainInitializer::ComputeTerrainVectors()
{
	// The ComputeTerrainVectors() function is used for traversing through the terrain model
	// and finding three vertices for each triangle. Then it takes those three vertices
	// and passes them into the CalculateTangentBinormal function to calculate the tangent
	// and binormal for that triangle. The tangent and binormal are passed back from the 
	// function by reference and then we copy them into the terrain model

	// earlier we've already computed normals for the terrain model so we don't have to do it again
	bool calcNormalsForTerrain = false;

	// for calculations of the the terrain tangent, binormal, etc.
	std::unique_ptr<ModelMath> pModelMath = std::make_unique<ModelMath>();

	// calculate tangent and binormal for the terrain
	pModelMath->CalculateModelVectors(this->verticesArr_, calcNormalsForTerrain);

	return;

} // end CalculateTerrainVectors

///////////////////////////////////////////////////////////

bool TerrainInitializer::LoadTerrainCells(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	std::vector<RenderableGameObject*> & terrainCellsArr,
	ModelInitializerInterface* pModelInitializer,
	ModelToShaderMediatorInterface* pModelToShaderMediator)
{
	// LoadTerrainCells is the function where we create the array of terrain cell objects.
	// We specify the size of the cell (for example: 33x33) and then create the array. 
	// Once the array is created we loop through it and initialize each cell with its part of 
	// the terrain model. We send a pointer to the terrain model and indices (i and j)
	// for the current position of the cell so that it knows where to read in the data from
	// the terrain model to build the current cell

	Log::Debug();

	TerrainCellClass* pTerrainCell = nullptr;
	bool result = false;



	std::string debugMsg{ "create and initialize " + std::to_string(pSetupData_->cellCount) + " terrain cells" };
	Log::Debug(debugMsg.c_str());

	try
	{
		// resize the array of terrain cells game objects
		terrainCellsArr.resize(pSetupData_->cellCount);

		// create a struct which we will use during creation of each terrain cell
		std::unique_ptr<TerrainCellClass::InitTerrainCellData> pDataForInit = std::make_unique<TerrainCellClass::InitTerrainCellData>();
		
		// fill in data structure with common initialization data and it remains the same for each cell
		// NOTE: description of these fields look in the InitTerrainCellData structure;
		//pDataForInit->cellWidth = cellWidth_;
		//pDataForInit->cellHeight = cellHeight_;
		//pDataForInit->terrainWidth = pSetupData_->terrainWidth;
		//pDataForInit->pModelToShaderMediator = pModelToShaderMediator;
		//pDataForInit->renderingShaderName = pSetupData_->renderingShaderName;
		pDataForInit->numVerticesInQuad = 6;
		pDataForInit->modelIndexStride = 
			(pDataForInit->terrainWidth * pDataForInit->numVerticesInQuad) - 
			(pDataForInit->cellWidth * pDataForInit->numVerticesInQuad);
		pDataForInit->quadWidthOfCell = pDataForInit->cellWidth - 1;    
		pDataForInit->quadHeightOfCell = pDataForInit->cellHeight - 1;  
		pDataForInit->vertexNumInCellRow = pDataForInit->quadWidthOfCell * pDataForInit->numVerticesInQuad;
		pDataForInit->verticesCountInCell = pDataForInit->quadHeightOfCell * pDataForInit->quadWidthOfCell * pDataForInit->numVerticesInQuad;

		// get paths to cell's default textures
		const std::string diffuseTexturePath{ Settings::Get()->GetString("TERRAIN_CELL_DEFAULT_DIFFUSE_TEXTURE_PATH") };
		const std::string normalsTexturePath{ Settings::Get()->GetString("TERRAIN_CELL_DEFAULT_NORMALS_TEXTURE_PATH") };

		// we have these default textures for all the terrain cells
		pDataForInit->texturesPaths.insert({ diffuseTexturePath, aiTextureType::aiTextureType_DIFFUSE });
		pDataForInit->texturesPaths.insert({ normalsTexturePath, aiTextureType::aiTextureType_NORMALS });

		// ---------------------------------------------------------------- //


		// loop through and initialize all the terrain cells
		for (UINT j = 0; j < cellRowCount; j++)
		{
			for (UINT i = 0; i < cellRowCount; i++)
			{
				// create a new empty terrain cell model
				pTerrainCell = new TerrainCellClass(pDevice, pDeviceContext);

				// cell's coordinates in the terrain
				pDataForInit->nodeIndexX = i;
				pDataForInit->nodeIndexY = j;

				// try to initialize this terrain cell model
				result = pTerrainCell->Initialize(pDataForInit.get(),  // we'll use this data for initialization of the cell
					this->verticesArr_,
					this->indicesArr_);             
				Assert::True(result, "can't initialize a terrain cell model");

				///////////////////////////////////////////

				// create a new renderable game object, initialize it with our new terrain cell model,
				// and set it into the terrain cells array
				terrainCellsArr[(cellRowCount * j) + i] = new RenderableGameObject(pTerrainCell);

				//std::string debugMsg{ "A terrain cell by index " + std::to_string(index) + " is created" };
				//Log::Debug(debugMsg.c_str());
			} // for
		} // for

	}
	catch (std::bad_alloc & e)
	{
		Log::Error(e.what());
		throw EngineException("can't allocate memory for the array of pointer to pointer to a terrain cell object");
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		Log::Error("can't load terrain cells");
		return false;
	}



	Log::Debug();

	return true;
}



///////////////////////////////////////////////////////////

#endif
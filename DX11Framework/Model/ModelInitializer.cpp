////////////////////////////////////////////////////////////////////
// Filename:      ModelInitializer.cpp
// Description:   a concrete implementation of the ModelInitializerInterface;
// 
// Created:       07.07.23
////////////////////////////////////////////////////////////////////
#include "../Model/ModelInitializer.h"
#include "../Model/ModelLoader.h"
#include "../Model/ModelMath.h"



ModelInitializer::ModelInitializer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	assert(pDevice != nullptr);
	assert(pDeviceContext != nullptr);

	this->pDevice_ = pDevice;
	this->pDeviceContext_ = pDeviceContext;
}





bool ModelInitializer::InitializeFromFile(ID3D11Device* pDevice,
	std::vector<Mesh*> & meshesArr,       // an array of meshes which have vertices/indices buffers that will be filled with vertices/indices data
	const std::string & filePath,   
	const std::string & modelDirPath)     // a path to directory which contains a data file for this model
{
	// this function initializes a new model from the file 
	// of type .blend, .fbx, .3ds, .obj, etc.

	// check input params
	COM_ERROR_IF_FALSE(filePath.empty() == false, "the input filePath is empty");
	COM_ERROR_IF_FALSE(modelDirPath.empty() == false, "the input modelDirPath is empty");

	try
	{
		Assimp::Importer importer;

		const aiScene* pScene = importer.ReadFile(filePath,
			aiProcess_Triangulate |
			aiProcess_ConvertToLeftHanded);

		// assert that we successfully read the data file 
		COM_ERROR_IF_FALSE(pScene, "can't read a model's data file: " + filePath);

		// copy a path to model's directory
		this->modelDirPath_ = modelDirPath;

		// load all the meshes/materials/textures of this model
		this->ProcessNode(meshesArr, pScene->mRootNode, pScene);
	
		// since we've loaded all the data of the model we clear the path to its directory
		this->modelDirPath_.clear();
	
#if 0

		// 1. In case if we haven't converted yet the model into the internal type 
		//    we convert this model from some external type (.fbx, .obj, etc.) to
		//    the engine internal model type (.txt)

		result = this->ConvertModelFromFile(pModelData->GetPathToDataFile(), modelFilename);
		COM_ERROR_IF_FALSE(result, "can't convert model from the file: " + modelFilename);

		// 2. Load model of the engine internal type from the file
		result = this->LoadModelDataFromFile(pModelData, filePath);
		COM_ERROR_IF_FALSE(result, "can't load model data from the file: " + filePath);

		// 3. after loading model from the file we have to do some math calculations
		this->ExecuteModelMathCalculations(pModelData);

#endif

	}
	catch (COMException & e)
	{
		std::string errorMsg{ "can't initialize a model from the file: " + filePath };

		Log::Error(e, true);
		Log::Error(THIS_FUNC, errorMsg.c_str());

		return false;
	}

	return true;

} // end InitializeFromFile




///////////////////////////////////////////////////////////////////////////////////////////
//                              PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////

void ModelInitializer::ProcessNode(std::vector<Mesh*> & meshesArr,
	aiNode* pNode, 
	const aiScene* pScene)
{
	// go through all the meshes in the current model's node
	for (UINT i = 0; i < pNode->mNumMeshes; i++)
	{
		// get the mesh
		aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];

		// handle this mesh and push it into the model's meshes array
		meshesArr.push_back(this->ProcessMesh(pMesh, pScene));
	}

	// go through all the child nodes of the current node and handle it
	for (UINT i = 0; i < pNode->mNumChildren; i++)
	{
		this->ProcessNode(meshesArr, pNode->mChildren[i], pScene);
	}

	return;
} // end Process Node

///////////////////////////////////////////////////////////

Mesh* ModelInitializer::ProcessMesh(aiMesh* pMesh, const aiScene* pScene)
{
	// data to fill
	std::vector<VERTEX> verticesArr(pMesh->mNumVertices);
	std::vector<UINT> indicesArr;

	// get vertices 
	for (UINT i = 0; i < pMesh->mNumVertices; i++)
	{
		// store vertex coords
		verticesArr[i].position.x = pMesh->mVertices[i].x;
		verticesArr[i].position.y = pMesh->mVertices[i].y;
		verticesArr[i].position.z = pMesh->mVertices[i].z;

		// if we have some texture coords for this vertex store it as well
		if (pMesh->mTextureCoords[0])
		{
			verticesArr[i].texture.x = static_cast<float>(pMesh->mTextureCoords[0][i].x);
			verticesArr[i].texture.y = static_cast<float>(pMesh->mTextureCoords[0][i].y);
		}
	}

	// get indices
	for (UINT i = 0; i < pMesh->mNumFaces; i++)
	{
		aiFace face = pMesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indicesArr.push_back(face.mIndices[j]);
			//indicesArr[i] = face.mIndices[j];
	}

	try
	{
		// after loading mesh vertices data from the data file
		// we have to do some math calculations with these vertices
		this->ExecuteModelMathCalculations(verticesArr);

		// create textures objects (array of it) by material data of this mesh
		aiMaterial* material = pScene->mMaterials[pMesh->mMaterialIndex];
		std::vector<std::unique_ptr<TextureClass>> texturesArr;
		LoadMaterialTextures(texturesArr, pDevice_, material, aiTextureType::aiTextureType_DIFFUSE, pScene);
		
		// create a new mesh obj
		Mesh* pNewMesh = new Mesh(this->pDevice_, this->pDeviceContext_,
			verticesArr,
			indicesArr,
			texturesArr);

		// and return it
		return pNewMesh;
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		Log::Error(THIS_FUNC, "can't create a mesh obj");
		COM_ERROR_IF_FALSE(false, "can't create a mesh obj");
	}

	return nullptr;

} // end ProcessMesh

///////////////////////////////////////////////////////////

void ModelInitializer::LoadMaterialTextures(
	std::vector<std::unique_ptr<TextureClass>> & materialTextures,
	ID3D11Device* pDevice,
	aiMaterial* pMaterial,
	aiTextureType textureType,
	const aiScene* pScene)
{
	// this function loads a texture by material data


	TextureStorageType storeType = TextureStorageType::Invalid;
	UINT textureCount = pMaterial->GetTextureCount(textureType);


	// ----------------------------------------------------------//

	if (textureCount == 0)      	// if there are no textures
	{
		storeType = TextureStorageType::None;
		aiColor3D aiColor(0.0f, 0.0f, 0.0f);

		switch (textureType)
		{
			// create a new diffuse texture with some particular color
			case aiTextureType_DIFFUSE:
			{
				pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
				if (aiColor.IsBlack())    // if color == black, just use grey
				{
					materialTextures.push_back(std::make_unique<TextureClass>(pDevice, Colors::UnloadedTextureColor, aiTextureType_DIFFUSE));
					return;
				}

				BYTE red   = (BYTE)aiColor.r * 255;
				BYTE green = (BYTE)aiColor.g * 255;
				BYTE blue  = (BYTE)aiColor.b * 255;

				std::unique_ptr<TextureClass> pTexture = std::make_unique<TextureClass>(pDevice, Color(red, green, blue), textureType);
				materialTextures.push_back(std::move(pTexture));
				return;

				break;

			} // case
		} // switch
	} // if


	// ----------------------------------------------------------//

	else   // we have some texture(s)
	{
		// go through each texture for this material
		for (UINT i = 0; i < textureCount; i++)
		{
			aiString path;
			pMaterial->GetTexture(textureType, i, &path);

			// determine what the texture storage type is
			TextureStorageType storeType = DetermineTextureStorageType(pScene, pMaterial, i, textureType);

			switch (storeType)
			{
				case TextureStorageType::Disk:
				{
					std::string filename = this->modelDirPath_ + '/' + path.C_Str();
					
					// create a new disk texture and push it into the textures array
					std::unique_ptr<TextureClass> pTexture = std::make_unique<TextureClass>(this->pDevice_, filename, textureType);
					materialTextures.push_back(std::move(pTexture));
					break;
				}
			} // switch
		} // for
	} // else



	if (materialTextures.size() == 0)
	{
		// create a new unhandled texture and push it into the textures array
		std::unique_ptr<TextureClass> pTexture = std::make_unique<TextureClass>(pDevice, Colors::UnhandledTextureColor, aiTextureType_DIFFUSE);
		materialTextures.push_back(std::move(pTexture));
	} // if
	
	return;

} // end LoadMaterialTextures

///////////////////////////////////////////////////////////

TextureStorageType ModelInitializer::DetermineTextureStorageType(const aiScene* pScene,
	aiMaterial* pMaterial, 
	UINT index,
	aiTextureType textureType)
{
	// this function determines all the possible texture storage types

	if (pMaterial->GetTextureCount(textureType) == 0)
		return TextureStorageType::None;

	// get path to the texture
	aiString path;
	pMaterial->GetTexture(textureType, index, &path);
	std::string texturePath = path.C_Str();

	// check if texture is an embedded indexed texture by seeing if the file path is an index #
	if (texturePath[0] == '*')
	{
		if (pScene->mTextures[0]->mHeight == 0)
		{
			return TextureStorageType::EmbeddedIndexCompressed;
		}
		else
		{
			assert("SUPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSES TEXTURES" && 0);
			return TextureStorageType::EmbeddedIndexNonCompressed;
		}
	}

	// check if texture is an embedded texture but not indexed (path will be the texture's name instead of #)
	if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
	{
		if (pTex->mHeight == 0)
		{
			return TextureStorageType::EmbeddedCompressed;
		}
		else
		{
			assert("SUPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSES TEXTURES" && 0);
			return TextureStorageType::EmbeddedNonCompressed;
		}
	}

	// lastly check if texture is a filepath by check for period before extension name
	if (texturePath.find('.') != std::string::npos)
	{
		return TextureStorageType::Disk;
	}

	return TextureStorageType::None;   // no texture exists

} // end DetermineTextureStorageType

///////////////////////////////////////////////////////////

#if 0

bool ModelInitializer::ConvertModelFromFile(const std::string & modelType,
	const std::string & modelFilename)
{
	// generate full path to the INPUT/OUTPUT model's data files
	//std::string relativePathToModelsDir{ Settings::Get()->GetSettingStrByKey("MODEL_DIR_PATH") };
	std::string fullPathToModelsDir{ PROJECT_DIR };
	std::string fullPathToModelInputDataFile{ fullPathToModelsDir + modelFilename + ".obj" };
	std::string fullPathToModelOutputDataFile{ fullPathToModelsDir + modelFilename + ".txt" };

	// make correction about slash symbols
	std::replace(fullPathToModelInputDataFile.begin(), fullPathToModelInputDataFile.end(), '\\', '/');
	std::replace(fullPathToModelOutputDataFile.begin(), fullPathToModelOutputDataFile.end(), '\\', '/');

	// check if we already have an OUTPUT data file for the model of such type
	std::ifstream fin(fullPathToModelOutputDataFile, std::ios::in | std::ios::binary);
	bool executeModelConvertation = executeModelConvertation = fin.fail();

	// try to convert the model
	if (executeModelConvertation)
	{
		// load a dll which has model convertation functionals
		UtilsForDLL utils(L"ModelConverterDLL.dll");

		// load an adress of the dll's function so we can call it later
		DLLPROC ImportModelFromFileFunc = utils.GetProcAddrFromDLL("ImportModelFromFile");

		if (ImportModelFromFileFunc == NULL)
		{
			COM_ERROR_IF_FALSE(false, "the function address is invalid");
		}

		// call the function for importing the model 
		(ImportModelFromFileFunc)(fullPathToModelInputDataFile.c_str(), fullPathToModelOutputDataFile.c_str());

	}
	
	return true;
}

///////////////////////////////////////////////////////////

bool ModelInitializer::LoadModelDataFromFile(ModelData* pModelData,
	const std::string & modelFilename)
{
	// Handles loading the model data from the text (.txt) file into the data object variable.
	// This model data MUST BE IN THE ENGINE INTERNAL MODEL TYPE which was converted from some
	// other model type (for instance: obj, fbx, 3dx, etc.)

	bool result = false;
	std::unique_ptr<ModelLoader> pModelLoader = std::make_unique<ModelLoader>();



	// try to load model's data of the engine internal model type
	result = pModelLoader->Load(modelFilename + ".txt",
		pModelData->GetVertices(),
		pModelData->GetIndices());
	COM_ERROR_IF_FALSE(result, "can't load model from file: " + modelFilename);

	// set the number of vertices/indices (both must be equal)
	//pModelData->SetVertexCount(pModelLoader->GetIndexCount());
	//pModelData->SetIndexCount(pModelLoader->GetIndexCount());

	return true;
}


#endif

///////////////////////////////////////////////////////////

void ModelInitializer::ExecuteModelMathCalculations(std::vector<VERTEX> & verticesArr)
{
	// is used for calculations of the model's normal vector, binormal, etc.
	std::unique_ptr<ModelMath> pModelMath = std::make_unique<ModelMath>(); 

	// after the model data has been loaded we now call the CalculateModelVectors() to
	// calculate the tangent and binormal. It also recalculates the normal vector;
	pModelMath->CalculateModelVectors(verticesArr);
}

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




// initialize a new model from the file of type .blend, .fbx, .3ds, .obj, etc.
bool ModelInitializer::InitializeFromFile(ID3D11Device* pDevice,
	std::vector<Mesh*> & meshesArr,       // an array of meshes which have vertices/indices buffers that will be filled with vertices/indices data
	const std::string & filePath)
{
	COM_ERROR_IF_FALSE(filePath.empty() == false, "the input filePath is empty");

	bool result = false;

	try
	{
		
		Assimp::Importer importer;

		const aiScene* pScene = importer.ReadFile(filePath,
			aiProcess_Triangulate |
			aiProcess_ConvertToLeftHanded);

		// assert that we successfully read the data file 
		COM_ERROR_IF_FALSE(pScene, "can't read a model's data file: " + filePath);

		this->ProcessNode(meshesArr, pScene->mRootNode, pScene);


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
}




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
		// create a new mesh obj
		Mesh* pMesh = new Mesh(this->pDevice_, this->pDeviceContext_,
			verticesArr,
			indicesArr);

		// and return it
		return pMesh;
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

///////////////////////////////////////////////////////////

void ModelInitializer::ExecuteModelMathCalculations(ModelData* pModelData)
{
	// is used for calculations of the model's normal vector, binormal, etc.
	std::unique_ptr<ModelMath> pModelMath = std::make_unique<ModelMath>(); 

	// after the model data has been loaded we now call the CalculateModelVectors() to
	// calculate the tangent and binormal. It also recalculates the normal vector;
	pModelMath->CalculateModelVectors(pModelData->GetVertices());
}

#endif
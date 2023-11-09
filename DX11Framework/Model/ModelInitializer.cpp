////////////////////////////////////////////////////////////////////
// Filename:      ModelInitializer.cpp
// Description:   a concrete implementation of the ModelInitializerInterface;
// 
// Created:       07.07.23
////////////////////////////////////////////////////////////////////
#include "../Model/ModelInitializer.h"
#include "../Model/ModelLoader.h"
#include "../Model/ModelMath.h"

#include <algorithm>                      // for using std::replace()


// initialize a new model from the file of type .blend, .fbx, .3ds, .obj, etc.
bool ModelInitializer::InitializeFromFile(ID3D11Device* pDevice,
	ModelData* pModelData,
	const std::string & modelFilename)
{
	assert(pModelData != nullptr);
	assert(!modelFilename.empty());

	bool result = false;

	try
	{
		// 1. In case if we haven't converted yet the model into the internal type 
		//    we convert this model from some external type (.fbx, .obj, etc.) to
		//    the engine internal model type (.txt)
		result = this->ConvertModelFromFile(pModelData->GetPathToDataFile(), modelFilename);
		COM_ERROR_IF_FALSE(result, "can't convert model from the file: " + modelFilename);

		// 2. Load model of the engine internal type from the file
		result = this->LoadModelDataFromFile(pModelData, modelFilename);
		COM_ERROR_IF_FALSE(result, "can't load model data from the file: " + modelFilename);

		// 3. after loading model from the file we have to do some math calculations
		this->ExecuteModelMathCalculations(pModelData);
	}
	catch (COMException & e)
	{
		std::string errorMsg{ "can't initialize a model from the file: " + modelFilename };

		Log::Error(e, true);
		Log::Error(THIS_FUNC, errorMsg.c_str());

		return false;
	}

	

	return true;
}

///////////////////////////////////////////////////////////

bool ModelInitializer::ConvertModelFromFile(const std::string & modelType,
	const std::string & modelFilename)
{
	// generate full path to the INPUT/OUTPUT model's data files
	std::string relativePathToModelsDir{ Settings::Get()->GetSettingStrByKey("MODEL_DIR_PATH") };
	std::string fullPathToModelsDir{ PROJECT_DIR + relativePathToModelsDir };
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

bool ModelInitializer::InitializeMesh(Mesh** ppMesh,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const std::vector<VERTEX> & verticesData,
	const std::vector<UINT> & indicesData)
{
	// this function creates and initializes a model's mesh with 
	// vertices and indices data

	try
	{
		*ppMesh = new Mesh(pDevice, pDeviceContext, verticesData, indicesData);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		Log::Error(THIS_FUNC, "can't allocate memory for a model's mesh");
		COM_ERROR_IF_FALSE(false, "can't allocate memory for a model's mesh");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't create a model's mesh");
		return false;
	}

	return true;

} // end InitializeMesh

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
	pModelData->SetVertexCount(pModelLoader->GetIndexCount());
	pModelData->SetIndexCount(pModelLoader->GetIndexCount());

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

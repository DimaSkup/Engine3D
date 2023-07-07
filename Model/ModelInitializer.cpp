////////////////////////////////////////////////////////////////////
// Filename:      ModelInitializer.cpp
// Description:   a concrete implementation of the ModelInitializerInterface;
// 
// Created:       07.07.23
////////////////////////////////////////////////////////////////////
#include "../Model/ModelInitializer.h"


// initialize a new model from the file of type .blend, .fbx, .3ds, .obj, etc.
bool ModelInitializer::InitializeFromFile(ID3D11Device* pDevice,
	ModelData* pModelData,
	const std::string & modelFilename)
{
	assert(pModelData != nullptr);
	assert(!modelFilename.empty());

	bool result = false;

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


	return true;
}



// initialize a new model's data using data of the another model
bool ModelInitializer::InitializeCopyOf(ModelData* pNewModelData,
	ModelData* pOriginModelData,
	ID3D11Device* pDevice)
{
	try
	{
		bool result = false;

		// copy model's data
		pNewModelData->SetID(pOriginModelData->GetID());
		pNewModelData->SetIndexCount(pOriginModelData->GetIndexCount());
		pNewModelData->CopyVerticesData(pOriginModelData->GetVerticesData(), pOriginModelData->GetVertexCount());
		pNewModelData->CopyIndicesData(pOriginModelData->GetIndicesData(), pOriginModelData->GetIndexCount());

		//string debugMsg = modelId + " is initialized!";
		//Log::Debug(THIS_FUNC, debugMsg.c_str());

		return true;
	}
	catch (COMException & e)
	{
		Log::Error(e);
		return false;
	}

	return true;
}






bool ModelInitializer::ConvertModelFromFile(const std::string & modelType,
	const std::string & modelFilename)
{
	bool executeModelConvertation = false;

	// if we want to convert file model data into the internal model format
	if (executeModelConvertation)
	{
		std::unique_ptr<ModelConverterClass> pModelConverter = std::make_unique<ModelConverterClass>();
		std::string pathToModelFile{ Settings::Get()->GetSettingStrByKey("MODEL_DIR_PATH") + modelType + ".obj" };

		bool result = pModelConverter->ConvertFromObj(pathToModelFile);
		COM_ERROR_IF_FALSE(result, "can't convert .obj into the internal model format");
	}


	return true;
}





// Handles loading the model data from the text file into the data object variable.
// This model data MUST HAVE THE ENGINE INTERNAL MODEL TYPE which was converted from some
// other model type (for instance: obj, fbx, 3dx, etc.)
bool ModelInitializer::LoadModelDataFromFile(ModelData* pModelData,
	const std::string & modelFilename)
{
	bool result = false;
	std::unique_ptr<ModelLoader> pModelLoader = std::make_unique<ModelLoader>();

	// try to load model of the engine internal model type
	result = pModelLoader->Load(modelFilename,
		pModelData->GetAddressOfVerticesData(),
		pModelData->GetAddressOfIndicesData());
	COM_ERROR_IF_FALSE(result, "can't load model from file: " + modelFilename);

	// set the number of the indices
	pModelData->SetIndexCount(pModelLoader->GetIndexCount());



	return true;
} /* LoadModelDataFromFile() */



  // Initialization of the vertex and index buffers with model's data
bool ModelInitializer::InitializeBuffers(ID3D11Device* pDevice,
	VertexBuffer<VERTEX>* pVertexBuffer,
	IndexBuffer* pIndexBuffer,
	ModelData* pModelData)
{
	// load vertex data into the buffer
	HRESULT hr = pVertexBuffer->InitializeDefault(pDevice,
		pModelData->GetVerticesData(),
		pModelData->GetVertexCount());
	COM_ERROR_IF_FAILED(hr, "can't initialize a default vertex buffer");

	// load index data into the buffer
	hr = pIndexBuffer->Initialize(pDevice,
		pModelData->GetIndicesData(),
		pModelData->GetIndexCount());
	COM_ERROR_IF_FAILED(hr, "can't initialize an index buffer");


	return true;
} /* InitializeBuffers() */




void ModelInitializer::ExecuteModelMathCalculations(ModelData* pModelData)
{
	std::unique_ptr<ModelMath> pModelMath = std::make_unique<ModelMath>(); // for calculations of the model's normal vector, binormal, etc.

																		   // after the model data has been loaded we now call the CalculateModelVectors() to
																		   // calculate the tangent and binormal. It also recalculates the normal vector;
	pModelMath->CalculateModelVectors(pModelData->GetVerticesData(), pModelData->GetVertexCount());
}

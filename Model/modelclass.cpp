/////////////////////////////////////////////////////////////////////
// Filename:        modelclass.cpp
// Description:     an implementation of the ModelClass class
// Last revising:   09.01.23
/////////////////////////////////////////////////////////////////////
#include "modelclass.h"










// ------------------------------------------------------------------------------ //
//
//                           PUBLIC METHODS
//
// ------------------------------------------------------------------------------ //

// The function here handle initializing of the model's vertex and 
// index buffers using some model data and texture


// initialize a copy of the model using the data of the original object
bool ModelClass::InitializeCopy(ModelClass* pModel, ID3D11Device* pDevice, const std::string& modelId)
{
	try
	{
		bool result = false;

		// copy model's data
		pData_->SetID(modelId);
		pData_->SetIndexCount(pModel->GetIndexCount());
		pData_->SetVerticesData(pModel->GetVerticesData(), pModel->GetVertexCount());
		pData_->SetIndexData(pModel->GetIndicesData(), pModel->GetIndexCount());

		// Initialize the vertex and index buffer that hold the geometry for the model
		result = this->InitializeBuffers(pDevice, GetVerticesData(), GetIndicesData(), GetVertexCount(), GetIndexCount());
		COM_ERROR_IF_FALSE(result, "can't initialize the buffers");

		//string debugMsg = modelId + " is initialized!";
		//Log::Debug(THIS_FUNC, debugMsg.c_str());

		return true;
	}
	catch (COMException& e)
	{
		Log::Error(e);
		return false;
	}
} /* Initialize(pModel, modelId) */














// allocate memory for the vertices/indices arrays;
// and setup the number of vertices and indices of this model
void ModelClass::AllocateVerticesAndIndicesArrays(UINT vertexCount, UINT indexCount)
{
	assert(vertexCount > 0);
	assert(indexCount > 0);

	pData_->AllocateVerticesAndIndicesArrays(vertexCount, indexCount);

	return;
}






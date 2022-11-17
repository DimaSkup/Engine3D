/////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
// Last revising: 29.03.22
/////////////////////////////////////////////////////////////////////
#include "modelclass.h"

using namespace std;

ModelClass::ModelClass(void)
{
}

ModelClass::ModelClass(const ModelClass& another)
{
}

ModelClass::~ModelClass(void)
{
}

// ------------------------------------------------------------------------------ //
//
//                           PUBLIC METHODS
//
// ------------------------------------------------------------------------------ //






// Shutting down of the model class, releasing of the memory, etc.
void ModelClass::Shutdown(void)
{
	ReleaseTexture();   // Release the model texture
	ShutdownBuffers();  // release the memory from the vertex and index buffer

	return;
}

// Put the vertex buffer data and index buffer data on the video card 
// to prepare this data for rendering
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);

	return;
}

// Get the number of indices
int ModelClass::GetIndexCount(void)
{
	return indexCount_;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return pTexture_->GetTexture();
}






// ------------------------------------------------------------------------------ //
//
//                           PRIVATE METHODS
//
// ------------------------------------------------------------------------------ //




// Releasing of the allocated memory from the vertex and index buffers
void ModelClass::ShutdownBuffers(void)
{
	_RELEASE(pIndexBuffer_);
	_RELEASE(pVertexBuffer_);

	return;
}


// This function prepares the vertex and index buffers for rendering
// sets up of the input assembler (IA) state
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;

	// set the vertex buffer as active
	deviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	// set the index buffer as active
	deviceContext->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);

	// set which type of primitive topology we want to use
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

// Creates the texture object and then initialize it with the input file name provided.
bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	// Create the texture object
	pTexture_ = new(std::nothrow) TextureClass;
	if (!pTexture_)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the texture object");
		return false;
	}

	// Initialize the texture object
	if (!pTexture_->Initialize(device, filename))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the texture object");
		return false;
	}

	return true;
}

// Releases the texture object that was created and loaded during the LoadTexture function
void ModelClass::ReleaseTexture(void)
{
	// Release the texture object
	_SHUTDOWN(pTexture_);

	return;
}

#include "model2D.h"

// initialize a model using only its vertices data
bool Model2D::Initialize(ID3D11Device* pDevice, VERTEX_2D* vertices)
{
	// Initialize the vertex and index buffer that hold the geometry for the model
	if (!InitializeBuffers(pDevice))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the buffers");
		return false;
	}

	return true;
}


// memory allocation (we need it because we use DirectX::XM-objects)
void* Model2D::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);
	if (!ptr)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for object");
		return nullptr;
	}

	return ptr;
}

void Model2D::operator delete(void* p)
{
	_aligned_free(p);
}



// initialize of the vertex and index buffer for some 2D model
bool Model2D::InitializeBuffers(ID3D11Device* pDevice)
{
	Log::Debug(THIS_FUNC_EMPTY);

	HRESULT hr = S_OK;

	return true;
}
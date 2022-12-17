////////////////////////////////////////////////////////////////////
// Filename:     SamplerState.h
// Description:  it is an implementation of the ID3D11SamplerState
// Revising:     24.11.22
////////////////////////////////////////////////////////////////////

#include "SamplerState.h"

// setup the sampler state description to the default values
SamplerState::SamplerState()
{
	Log::Debug(THIS_FUNC_EMPTY);

	// setup the default description for the sampler state which is used in the pixel HLSH shader
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER; //D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.MipLODBias = 0.0f;
}


// create a sampler state using the description
bool SamplerState::Initialize(ID3D11Device* pDevice)
{
	Log::Debug(THIS_FUNC_EMPTY);

	HRESULT hr = S_OK;

	// create a sampler state using the description
	hr = pDevice->CreateSamplerState(&samplerDesc, &pSamplerState_);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the sampler state");
		return false;
	}

	return true;
}

ID3D11SamplerState* SamplerState::GetSampler()
{
	return pSamplerState_;
}

// returns a pointer to pointer to the sampler state
ID3D11SamplerState* const* SamplerState::GetAddressOf()
{
	return &pSamplerState_;
}
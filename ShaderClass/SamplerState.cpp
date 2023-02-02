////////////////////////////////////////////////////////////////////
// Filename:     SamplerState.h
// Description:  it is an implementation of the ID3D11SamplerState
// Revising:     24.11.22
////////////////////////////////////////////////////////////////////

#include "SamplerState.h"



// create a sampler state using the description
bool SamplerState::Initialize(ID3D11Device* pDevice)
{
	//Log::Debug(THIS_FUNC_EMPTY);

	/*  OLD STYLE

	D3D11_SAMPLER_DESC samplerDesc;           

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
	*/


	// setup the sampler description
	CD3D11_SAMPLER_DESC samplerDesc(D3D11_DEFAULT);
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;

	// create a sampler state
	HRESULT hr = pDevice->CreateSamplerState(&samplerDesc, &pSamplerState_);
	COM_ERROR_IF_FAILED(hr, "can't create the sampler state");

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
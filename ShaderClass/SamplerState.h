////////////////////////////////////////////////////////////////////
// Filename:     SamplerState.h
// Description:  it is an implementation of the ID3D11SamplerState
// Revising:     24.11.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>

#include "../Engine/macros.h"
#include "../Engine/Log.h"

class SamplerState
{
public:
	SamplerState();                               // setup the sampler state description to the default values
	bool Initialize(ID3D11Device* pDevice);

	ID3D11SamplerState* GetSampler();             
	ID3D11SamplerState* const*  GetPPSampler();   // returns a pointer to pointer to the sampler state (is used during the setting up of the pixels shaders)

	D3D11_SAMPLER_DESC samplerDesc;               // we make desc as public for handful changing of the description parameters directly

private:
	ID3D11SamplerState* pSamplerState_;
};

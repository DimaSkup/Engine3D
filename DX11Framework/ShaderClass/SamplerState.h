////////////////////////////////////////////////////////////////////
// Filename:     SamplerState.h
// Description:  functional for work with ID3D11SamplerState
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
	SamplerState();

	bool Initialize(ID3D11Device* pDevice, D3D11_SAMPLER_DESC* pSamplerDesc = nullptr);

	ID3D11SamplerState* GetSampler();             
	ID3D11SamplerState* const*  GetAddressOf();   // returns a double pointer to the sampler state (is used during the setting up of the pixels shaders)

private:
	ID3D11SamplerState* pSamplerState_;
};

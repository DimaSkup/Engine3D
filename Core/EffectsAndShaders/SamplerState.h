////////////////////////////////////////////////////////////////////
// Filename:     SamplerState.h
// Description:  functional for work with ID3D11SamplerState
// 
////////////////////////////////////////////////////////////////////
#pragma once

#include <d3d11.h>


class SamplerState
{
public:
	SamplerState() {};

	bool Initialize(ID3D11Device* pDevice, D3D11_SAMPLER_DESC* pSamplerDesc = nullptr);

	inline ID3D11SamplerState* GetSampler() { return samplerState_; };
	inline ID3D11SamplerState* const* GetAddressOf() { return &samplerState_; };

private:
	ID3D11SamplerState* samplerState_ = nullptr;
};

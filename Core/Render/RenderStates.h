// *********************************************************************************
// Filename:     RenderStates.h
// Description:  defines render state objects
// 
// Created:      09.09.24
// *********************************************************************************
#pragma once

#include <d3d11.h>
#include <map>
#include <vector>

class RenderStates
{
public:
	enum STATES
	{
		// rasterizer params
		FILL_MODE_SOLID,
		FILL_MODE_WIREFRAME,
		CULL_MODE_BACK,
		CULL_MODE_FRONT,
		CULL_MODE_NONE,

		// blending states
		NO_COLOR_WRITE,
		ALPHA_DISABLE,
		ALPHA_ENABLE,
		ADDING,
		SUBTRACTING,
		MULTIPLYING,
		TRANSPARENCY,

		// depth stencil states
		DEPTH_ENABLED,
		DEPTH_DISABLED,
		MARK_MIRROR,     // for rendering mirror reflections
		DRAW_REFLECTION,
		NO_DOUBLE_BLEND,

		LAST   // to make possible iteration over the enum
	};

public:
	RenderStates();
	~RenderStates();

	void InitAll(ID3D11Device* pDevice);
	void DestroyAll();

	
	ID3D11DepthStencilState* GetDSS(const STATES state);


	// returns a hash to the pointer of the current rasterizer state
	inline uint8_t GetCurrentRSHash() const { return rasterStateHash_; }

	// set RS
	void SetRasterState(ID3D11DeviceContext* pDeviceContext, const STATES state);
	void SetRasterState(ID3D11DeviceContext* pDeviceContext, const std::vector<STATES>& states);
	void SetRasterStateByHash(ID3D11DeviceContext* pDeviceContext, const uint8_t hash);

	// set BS
	void SetBlendState(ID3D11DeviceContext* pDeviceContext, const STATES key);

private:
	void InitAllRasterParams(ID3D11Device* pDevice);
	void InitAllBlendStates(ID3D11Device* pDevice);
	void InitAllDepthStencilStates(ID3D11Device* pDevice);

	inline void ResetRasterStateHash() { rasterStateHash_ &= 0; }
	inline void TurnOnRasterParam(const STATES rsParam) { rasterStateHash_ |= (1 << rsParam);  }
	//inline void turnOffRasterParam(const STATES rsParam) { rasterStateHash_ &= ~(1 << rsParam); }

	ID3D11RasterizerState* GetRasterStateByHash(const uint8_t hash);

	void UpdateRSHash(const std::vector<STATES>& rsParams);
	void PrintErrAboutRSHash(const uint8_t hash);

private:
	std::map<STATES,  ID3D11BlendState*>        blendStates_;
	std::map<uint8_t, ID3D11RasterizerState*>   rasterStates_;
	std::map<STATES, ID3D11DepthStencilState*>  depthStencilStates_;

	// rasterizer state related stuff
	uint8_t rasterStateHash_     { 0b0000'0000 };              // hash to particular rasterizer state
	uint8_t turnOffCullModesHash_{ 0b1111'1111 };              // using this hash we turn off ALL the CULL modes at the same time
	uint8_t turnOffFillModesHash_{ 0b1111'1111 };              // using this hash we turn off ALL the FILL modes at the same time

};

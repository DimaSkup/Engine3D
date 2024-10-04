// *********************************************************************************
// Filename:     RenderStates.h
// Description:  constains implementation of the RenderStates class' functional
// 
// Created:      09.09.24
// *********************************************************************************
#include "RenderStates.h"
#include "../Common/MemHelpers.h"
#include "../Common/Assert.h"
#include "../Engine/log.h"

#include <sstream>

RenderStates::RenderStates() {}
RenderStates::~RenderStates() { DestroyAll(); }


// ********************************************************************************
//                            PUBLIC METHODS
// ********************************************************************************

void RenderStates::InitAll(ID3D11Device* pDevice)
{
	for (int idx = 0; idx < STATES::LAST; ++idx)
		blendStates_.insert({ (STATES)idx, nullptr });

	InitAllRasterParams(pDevice);
	InitAllBlendStates(pDevice);
	InitAllDepthStencilStates(pDevice);

	// init some hashes to use it later during switching between some states
	turnOffFillModesHash_ &= ~(1 << FILL_MODE_SOLID);
	turnOffFillModesHash_ &= ~(1 << FILL_MODE_WIREFRAME);

	turnOffCullModesHash_ &= ~(1 << CULL_MODE_BACK);
	turnOffCullModesHash_ &= ~(1 << CULL_MODE_FRONT);
	turnOffCullModesHash_ &= ~(1 << CULL_MODE_NONE);
}

///////////////////////////////////////////////////////////

void RenderStates::DestroyAll()
{
	for (auto& it : blendStates_)
		SafeRelease(&it.second);

	for (auto& it : rasterStates_)
		SafeRelease(&it.second);

	for (auto& it : depthStencilStates_)
		SafeRelease(&it.second);
		
	blendStates_.clear();
	rasterStates_.clear();
	depthStencilStates_.clear();
}

///////////////////////////////////////////////////////////

ID3D11DepthStencilState* RenderStates::GetDSS(const STATES state)
{
	// return a ptr to the depth stencil state by state enum key

	try
	{
		return depthStencilStates_.at(state);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(e.what());
		Log::Error("there is no depth stencil state by key: " + std::to_string(state));
		return nullptr;
	}
}

#if 0
ID3D11BlendState* RenderStates::GetBlendState(const STATES key)
{
	switch (key)
	{
		case NO_COLOR_WRITE:
		case ALPHA_DISABLE:
		case ALPHA_ENABLE:
		case ADDING:
		case SUBTRACTING:
		case MULTIPLYING:
		case TRANSPARENCY:
		{
			return static_cast<ID3D11BlendState*>(blendStates_[key]);
		}
		default:
		{
			Log::Error("unknown blend state key: " + std::to_string(key));
			return nullptr;
		}
	}
}
#endif
///////////////////////////////////////////////////////////

void RenderStates::SetRasterState(ID3D11DeviceContext* pDeviceContext, const STATES state)
{
	SetRasterState(pDeviceContext, std::vector<STATES>{state});
}

///////////////////////////////////////////////////////////

void RenderStates::SetRasterState(ID3D11DeviceContext* pDeviceContext, const std::vector<STATES>& states)
{
	// set up a raster state according to the input states params

	UpdateRSHash(states);
	pDeviceContext->RSSetState(GetRasterStateByHash(GetCurrentRSHash()));
}

///////////////////////////////////////////////////////////

void RenderStates::SetRasterStateByHash(ID3D11DeviceContext* pDeviceContext, const uint8_t hash)
{
	// set up a raster state according to the input hash

	ID3D11RasterizerState* pRS = nullptr;

	// if the RS by input hash is valid AND we don't want to set the same RS
	if ((rasterStateHash_ != hash) && (pRS = GetRasterStateByHash(hash)))
	{
		pDeviceContext->RSSetState(pRS);
		rasterStateHash_ = hash;
	}
}

///////////////////////////////////////////////////////////

void RenderStates::SetBlendState(ID3D11DeviceContext* pDeviceContext, const STATES key)
{
	switch (key)
	{
		case NO_COLOR_WRITE:
		case ALPHA_DISABLE:
		case ALPHA_ENABLE:
		case ADDING:
		case SUBTRACTING:
		case MULTIPLYING:
		{
			pDeviceContext->OMSetBlendState((ID3D11BlendState*)blendStates_[key], NULL, 0xFFFFFFFF);
			break;
		}
		case TRANSPARENCY:
		{
			//float blendFactor[4] = { 0,0,0,0 };
			float blendFactor[4] = { 0.5f,0.5f,0.5f,0.5f };
			pDeviceContext->OMSetBlendState((ID3D11BlendState*)blendStates_[key], blendFactor, 0xFFFFFFFF);
			break;
		}
		default:
		{
			Log::Error("unknown blend state: " + std::to_string(key));
		}
	}
}


// ********************************************************************************
//                            PRIVATE METHODS
// ********************************************************************************

void RenderStates::InitAllRasterParams(ID3D11Device* pDevice)
{
	// THIS FUNCTION creates/sets up the rasterizer state objects
	// 
	// firstly we set up a description for a raster state, then we create it
	// after creation we generate a hash for this particular raster state
	// and insert into the map created pairs ['hash' => 'ptr_to_raster_state']

	try
	{
		HRESULT hr = S_OK;
		ID3D11RasterizerState* pRasterState = nullptr;
		CD3D11_RASTERIZER_DESC pRasterDesc(D3D11_DEFAULT);

		//
		// 1. create a fill solid + cull back rasterizer state
		//
		pRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		pRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

		hr = pDevice->CreateRasterizerState(&pRasterDesc, &pRasterState);
		Assert::NotFailed(hr, "can't create a raster state: fill solid + cull back");

		UpdateRSHash({ FILL_MODE_SOLID, CULL_MODE_BACK });
		rasterStates_.insert({ GetCurrentRSHash(), pRasterState });


		//
		// 2. create a fill solid + cull front rasterizer state
		//
		pRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		pRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;

		hr = pDevice->CreateRasterizerState(&pRasterDesc, &pRasterState);
		Assert::NotFailed(hr, "can't create a raster state: fill solid + cull front");

		ResetRasterStateHash();
		UpdateRSHash({ FILL_MODE_SOLID, CULL_MODE_FRONT });
		rasterStates_.insert({ GetCurrentRSHash(), pRasterState });


		//
		// 3. create a fill wireframe + cull back rasterizer state
		//
		pRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		pRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

		hr = pDevice->CreateRasterizerState(&pRasterDesc, &pRasterState);
		Assert::NotFailed(hr, "can't create a raster state: fill wireframe + cull back");

		ResetRasterStateHash();
		UpdateRSHash({ FILL_MODE_WIREFRAME, CULL_MODE_BACK });
		rasterStates_.insert({ GetCurrentRSHash(), pRasterState });


		//
		// 4. create a fill wireframe + cull front rasterizer state
		//
		pRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		pRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;

		hr = pDevice->CreateRasterizerState(&pRasterDesc, &pRasterState);
		Assert::NotFailed(hr, "can't create a raster state: fill wireframe + cull front");

		ResetRasterStateHash();
		UpdateRSHash({ FILL_MODE_WIREFRAME, CULL_MODE_FRONT });
		rasterStates_.insert({ GetCurrentRSHash(), pRasterState });


		//
		// 5. create a fill wireframe + cull none rasterizer state
		//
		pRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		pRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

		hr = pDevice->CreateRasterizerState(&pRasterDesc, &pRasterState);
		Assert::NotFailed(hr, "can't create a raster state: fill solid + cull none");

		ResetRasterStateHash();
		UpdateRSHash({ FILL_MODE_WIREFRAME, CULL_MODE_NONE });
		rasterStates_.insert({ GetCurrentRSHash(), pRasterState });

		//
		// 6. create a fill solid + cull none rasterizer state
		//
		pRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		pRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

		hr = pDevice->CreateRasterizerState(&pRasterDesc, &pRasterState);
		Assert::NotFailed(hr, "can't create a raster state: fill solid + cull none");

		ResetRasterStateHash();
		UpdateRSHash({ FILL_MODE_SOLID, CULL_MODE_NONE });
		rasterStates_.insert({ GetCurrentRSHash(), pRasterState });


		//
		// AFTER ALL: reset the rasterizer state hash after initialization
		//            and set the default params
		ResetRasterStateHash();
		UpdateRSHash({ FILL_MODE_SOLID, CULL_MODE_BACK });
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		throw EngineException("can't initialize the rasterizer state");
	}
}

///////////////////////////////////////////////////////////

void RenderStates::InitAllBlendStates(ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;
	D3D11_BLEND_DESC blendDesc { 0 };
	D3D11_RENDER_TARGET_BLEND_DESC& rtbd = blendDesc.RenderTarget[0];

	//
	// Disabled blending
	//

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;

	rtbd.BlendEnable    = FALSE;
	rtbd.SrcBlend       = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp        = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha  = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = pDevice->CreateBlendState(&blendDesc, (ID3D11BlendState**)&blendStates_[ALPHA_DISABLE]);
	Assert::NotFailed(hr, "can't create an alpha disabled blending state");


	//
	// Enabled blending (for rendering fonts, sky plane, ...)
	//

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;

	rtbd.BlendEnable    = TRUE;
	rtbd.SrcBlend       = D3D11_BLEND_ONE;
	rtbd.DestBlend      = D3D11_BLEND_ONE;
	rtbd.BlendOp        = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha  = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = pDevice->CreateBlendState(&blendDesc, (ID3D11BlendState**)&blendStates_[ALPHA_ENABLE]);
	Assert::NotFailed(hr, "can't create an alpha enabled blending state");


	//
	// Adding BS
	//

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;

	rtbd.BlendEnable    = TRUE;
	rtbd.SrcBlend       = D3D11_BLEND_ONE;
	rtbd.DestBlend      = D3D11_BLEND_ONE;
	rtbd.BlendOp        = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha  = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = pDevice->CreateBlendState(&blendDesc, (ID3D11BlendState**)&blendStates_[ADDING]);
	Assert::NotFailed(hr, "can't create an adding blend state");


	//
	// Subtracting BS
	//

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;

	rtbd.BlendEnable    = TRUE;
	rtbd.SrcBlend       = D3D11_BLEND_ONE;
	rtbd.DestBlend      = D3D11_BLEND_ONE;
	rtbd.BlendOp        = D3D11_BLEND_OP_SUBTRACT;
	rtbd.SrcBlendAlpha  = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = pDevice->CreateBlendState(&blendDesc, (ID3D11BlendState**)&blendStates_[SUBTRACTING]);
	Assert::NotFailed(hr, "can't create a subtracting blend state");


	//
	// Multiplying BS
	//

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;

	rtbd.BlendEnable    = TRUE;
	rtbd.SrcBlend       = D3D11_BLEND_ZERO;
	rtbd.DestBlend      = D3D11_BLEND_SRC_COLOR;
	rtbd.BlendOp        = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha  = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = pDevice->CreateBlendState(&blendDesc, (ID3D11BlendState**)&blendStates_[MULTIPLYING]);
	Assert::NotFailed(hr, "can't create a multiplying blend state");


	// 
	// Transparent BS
	//

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;

	rtbd.BlendEnable    = true;
	rtbd.SrcBlend       = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp        = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha  = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = pDevice->CreateBlendState(&blendDesc, (ID3D11BlendState**)&blendStates_[TRANSPARENCY]);
	Assert::NotFailed(hr, "can't create a transparent blend state");
}

///////////////////////////////////////////////////////////

void RenderStates::InitAllDepthStencilStates(ID3D11Device* pDevice)
{
	// initialize different depth stencil states
	
	HRESULT hr = S_OK;

	//
	// depth ENABLED
	//

	CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	hr = pDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilStates_[DEPTH_ENABLED]);
	Assert::NotFailed(hr, "can't create a depth stencil state");


	//
	// depth DISABLED (for 2D rendering)
	//

	CD3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc(D3D11_DEFAULT);
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.DepthEnable = false;

	hr = pDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &depthStencilStates_[DEPTH_DISABLED]);
	Assert::NotFailed(hr, "can't create the depth disabled stencil state");


	// mark mirror:
	// this state is used to mark the position of a mirror on the stencil buffer, 
	// without changing the depth buffer. We will pair this with a new BlendState 
	// (noRenderTargetWritesBS) which will disable writing any color information 
	// to the backbuffer, so that we will have the combined effect which will 
	// be used to write only to the stencil.

	CD3D11_DEPTH_STENCIL_DESC markMirrorDSSDesc(D3D11_DEFAULT);

	markMirrorDSSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	markMirrorDSSDesc.StencilEnable = TRUE;
	markMirrorDSSDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	markMirrorDSSDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;

	hr = pDevice->CreateDepthStencilState(&markMirrorDSSDesc, &depthStencilStates_[MARK_MIRROR]);
	Assert::NotFailed(hr, "can't create a mark mirror depth stencil state");


	// draw reflection:
	// this state will be used to draw the geometry that should appear as a reflection
	// in mirror. We will set the stencil test up so that we will only render pixels
	// if they have been previously marked as part of the mirror by the MarkMirrorDSS.

	CD3D11_DEPTH_STENCIL_DESC drawReflectionDSSDesc(D3D11_DEFAULT);

	drawReflectionDSSDesc.StencilEnable = TRUE;
	drawReflectionDSSDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDSSDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	drawReflectionDSSDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDSSDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	hr = pDevice->CreateDepthStencilState(&drawReflectionDSSDesc, &depthStencilStates_[DRAW_REFLECTION]);
	Assert::NotFailed(hr, "can't create a draw reflection depth stencil state");

	// no double blending:
	// this state will be used to draw our shadown. Because we are drawing our shadows as
	// partially transparent black using alpha-blending, if we were to simply draw the 
	// shadow geometry, we would have darker patches where multiple surfaces of the shadow
	// object are projected to the shadow plane, a condition known as shadow-acne. Instead,
	// we setup the stencil test to check that the current stencil value is equal to the 
	// reference value, and increment on passes. Thus, the first time a projected pixel is
	// drawn, it will pass the stencil test, increment the stencil value, and be rendered.
	// On subsequent draws, the pixel will fail the stencil test.

	CD3D11_DEPTH_STENCIL_DESC noDoubleBlendDSSDesc(D3D11_DEFAULT);

	noDoubleBlendDSSDesc.StencilEnable = TRUE;
	noDoubleBlendDSSDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlendDSSDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	noDoubleBlendDSSDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlendDSSDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	hr = pDevice->CreateDepthStencilState(&noDoubleBlendDSSDesc, &depthStencilStates_[NO_DOUBLE_BLEND]);
	Assert::NotFailed(hr, "can't create a no double blend depth stencil state");
}

///////////////////////////////////////////////////////////

ID3D11RasterizerState* RenderStates::GetRasterStateByHash(const uint8_t hash)
{
	// return a pointer to some rasterizer state by hash

	auto iterator = rasterStates_.find(hash);

	// if we found a rasterizer state by the hash
	if (iterator != rasterStates_.end())
	{
		return iterator->second;
	}

	// we didn't found any rasterizer state
	else
	{
		PrintErrAboutRSHash(hash);
		return nullptr;
	}
}

///////////////////////////////////////////////////////////

void RenderStates::UpdateRSHash(const std::vector<STATES>& rsParams)
{
	// setup the rasterizer state hash according to the params 

	for (const STATES param : rsParams)
	{
		switch (param)
		{
			// switch between rasterizer fill modes
			case FILL_MODE_SOLID:
			case FILL_MODE_WIREFRAME:
			{
				rasterStateHash_ &= turnOffFillModesHash_;   // turn off all the fill modes
				TurnOnRasterParam(param);
				break;
			}
			// switch between rasterizer culling modes
			case CULL_MODE_BACK:
			case CULL_MODE_FRONT:
			case CULL_MODE_NONE:
			{
				rasterStateHash_ &= turnOffCullModesHash_;   // turn off all the cull modes
				TurnOnRasterParam(param);
				break;
			}
			default:
			{
				Log::Error("an unknown rasterizer state parameter: " + std::to_string(param));
				return;
			}
		}
	}	
}

///////////////////////////////////////////////////////////

void RenderStates::PrintErrAboutRSHash(const uint8_t hash)
{
	// if we got somewhere some wrong hash we call this method to 
	// print an error message about it

	std::stringstream hashStream;
	std::stringstream rsNames;
	rsNames << '\n';

	std::map<STATES, std::string> rasterParamsNames_ =
	{
		{FILL_MODE_SOLID, "FILL_MODE_SOLID"},
		{FILL_MODE_WIREFRAME, "FILL_MODE_WIREFRAME"},
		{CULL_MODE_BACK, "CULL_MODE_BACK"},
		{CULL_MODE_FRONT, "CULL_MODE_FRONT"},
		{CULL_MODE_NONE, "CULL_MODE_NONE"},
	};

	// print the hash
	int symbol = 0;
	for (int i = 7; i >= 0; i--)
	{
		// generate a string with the hash in binary view
		symbol = (hash >> i) & 1;
		hashStream << symbol << " ";

		// if the current symbol == 1 we get its shift in the hash (value of i)
		// and get a name of the rasterizer state parameter from the map
		if (symbol == 1)
			rsNames << rasterParamsNames_.at((STATES)i) << "\n";
	}

	Log::Error("can't get a raster state ptr by hash: " + hashStream.str());
	Log::Error("which is responsible to such params (at the same time):");
	Log::Error(rsNames.str());
}




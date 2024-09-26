// *********************************************************************************
// Filename:      InitRender.cpp
// Description:   implementation of functional for initialization 
//                of the Render module elements
// 
// Created:       30.08.24
// *********************************************************************************
#include "InitRender.h"
#include "Common/log.h"

namespace Render
{

bool InitRender::InitializeShaders(
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	ShadersContainer& shadersContainer,
	const DirectX::XMMATRIX& WVO)                 // world * base view * ortho
{
	// THIS FUNC initializes all the shader classes (color, texture, light, etc.)
	// and the HLSL shaders as well

	Log::Print("---------------------------------------------------------");
	Log::Print("                INITIALIZATION: SHADERS                  ");
	Log::Print("---------------------------------------------------------");

	try
	{
		bool result = false;

		result = shadersContainer.colorShader_.Initialize(pDevice, pDeviceContext);
		Assert::True(result, "can't initialize the color shader class");

		result = shadersContainer.textureShader_.Initialize(pDevice, pDeviceContext);
		Assert::True(result, "can't initialize the texture shader class");

		result = shadersContainer.lightShader_.Initialize(pDevice, pDeviceContext);
		Assert::True(result, "can't initialize the light shader class");

		result = shadersContainer.fontShader_.Initialize(pDevice, pDeviceContext, WVO);
		Assert::True(result, "can't initializer the font shader class");

		shadersContainer.lightShader_.SetFogParams(
			pDeviceContext,
			{ 0.4f, 0.6f, 0.8f }, // fog color
			30, 300);              // fog start and fog range
	}
	catch (std::bad_alloc& e)
	{
		Log::Error(e.what());
		return false;
	}
	catch (LIB_Exception& e) // if we have some error during initialization of shaders we handle such an error here
	{
		Log::Error(e, true);
		return false;
	}


	return true;
}

} // namespace Render
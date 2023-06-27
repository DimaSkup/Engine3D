////////////////////////////////////////////////////////////////////
// Filename:      SkyPlaneClass.h
// Description:   encapsulates everything related to the plane used
//                for rendering the clouds. It holds the geometry for
//                the sky plane, the two bitmaps textures for the 
//                clouds, and all the variables for the shader that
//                relate to how to draw the sky plane.
// 
// Created:       24.06.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "modelclass.h"


//////////////////////////////////
// Class name: SkyPlaneClass.h
//////////////////////////////////
class SkyPlaneClass : private ModelClass
{
public:
	SkyPlaneClass();
	~SkyPlaneClass();

	bool Initialize(ID3D11Device* pDevice, WCHAR* textureFilename1, WCHAR* textureFilename2);
	void Frame();
	void Render(ID3D11DeviceContext* pDeviceContext);

	float GetBrightness() const;
	float GetTranslation(UINT index) const;

private:  // restrict a copying of this class instance
	SkyPlaneClass(const SkyPlaneClass & obj);
	SkyPlaneClass & operator= (const SkyPlaneClass & obj);

private:
	bool InitializeSkyPlane(ID3D11Device* pDevice, int skyPlaneResolution, float skyPlaneWidth, float skyPlaneTop, float skyPlaneBottom, int textureRepeat);
	bool LoadTextures(ID3D11Device* pDevice, WCHAR* textureFilename1, WCHAR* textureFilename2);

private:
	float translationSpeed_[4] = { 0.0f };
	float textureTranslation_[4] = { 0.0f };
	ModelClass* pModel_ = nullptr;   // for using model functional

	float brightness_ = 0.0f;        // the brightness of the clouds is stores here 
								     // and set in the pixel shader during rendering
};

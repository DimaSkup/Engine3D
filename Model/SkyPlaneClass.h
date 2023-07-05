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
#include <vector>

#include "modelclass.h"


//////////////////////////////////
// Class name: SkyPlaneClass.h
//////////////////////////////////
class SkyPlaneClass final : public GraphicsComponent, public Model
{
public:
	SkyPlaneClass();
	~SkyPlaneClass();

	virtual bool Initialize(ID3D11Device* pDevice) override;
	void Frame();
	//void Render(ID3D11DeviceContext* pDeviceContext);

	bool LoadCloudTextures(ID3D11Device* pDevice, WCHAR* textureFilename1, WCHAR* textureFilename2);
	void SetTextureByIndex(WCHAR* textureFilename, UINT index);

	float* GetPtrToBrightness();
	std::vector<float>* GetPtrToTranslationData();

private:  // restrict a copying of this class instance
	SkyPlaneClass(const SkyPlaneClass & obj);
	SkyPlaneClass & operator= (const SkyPlaneClass & obj);

private:
	bool InitializeSkyPlane(ID3D11Device* pDevice, int skyPlaneResolution, float skyPlaneWidth, float skyPlaneTop, float skyPlaneBottom, int textureRepeat);
	
private:
	std::string modelType_{ "sky_plane" };

	// clouds translation data where index: 
	// 0 - 1st cloud X-axis; 1 - 1st cloud Z-axis; 
	// 2 - 2nd cloud X-axis; 3 - 2nd cloud Z-axis;
	std::vector<float> translationSpeed_ { 0.0f, 0.0f, 0.0f, 0.0f };

	// the current translation for the two textures
	std::vector<float> textureTranslation_ { 0.0f, 0.0f, 0.0f, 0.0f };

	ModelClass* pModel_ = nullptr;   // for using model functional
	float brightness_ = 0.0f;        // the brightness of the clouds is stores here 
								     // and set in the pixel shader during rendering
};

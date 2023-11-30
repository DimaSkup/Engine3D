////////////////////////////////////////////////////////////////////////////////////////////
// Filename:      SkyPlaneClass.h
// Description:   encapsulates everything related to the plane used
//                for rendering the clouds. It holds the geometry for
//                the sky plane, the two bitmaps textures for the 
//                clouds, and all the variables for the shader that
//                relate to how to draw the sky plane.
// 
// Created:       24.06.23
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <vector>

#include "Model.h"


//////////////////////////////////
// Class name: SkyPlaneClass.h
//////////////////////////////////
class SkyPlaneClass final : public Model
{
public:
	SkyPlaneClass(ModelInitializerInterface* pModelInitializer,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext);
	~SkyPlaneClass();

	virtual bool Initialize(const std::string & filePath) override;

	void Frame(float deltaTime);

	bool LoadCloudTextures(ID3D11Device* pDevice, const WCHAR* textureFilename1, const WCHAR* textureFilename2);
	void SetTextureByIndex(WCHAR* textureFilename, UINT index);

	float* GetTranslationData() _NOEXCEPT;
	const float GetBrightness() const _NOEXCEPT;

private:  // restrict a copying of this class instance
	SkyPlaneClass(const SkyPlaneClass & obj);
	SkyPlaneClass & operator= (const SkyPlaneClass & obj);

private:
	bool BuildSkyPlaneGeometry(ID3D11Device* pDevice,
		const int skyPlaneResolution, 
		const float skyPlaneWidth, 
		const float skyPlaneTop,
		const float skyPlaneBottom,
		const int textureRepeat);

	bool FillSkyPlaneArrays(ID3D11Device* pDevice,
		const int skyPlaneResolution,
		std::vector<VERTEX> & verticesArr,
		std::vector<UINT> & indicesArr);
private:
	// clouds translation data where index: 
	// 0 - 1st cloud X-axis; 1 - 1st cloud Z-axis; 
	// 2 - 2nd cloud X-axis; 3 - 2nd cloud Z-axis;
	std::vector<float> translationSpeed_ { 0.0f, 0.0f, 0.0f, 0.0f };

	// the current translation for the two textures
	float textureTranslation_[4] = { 0.0f };

	//Model* pModel_ = nullptr;        // for using model functional
	float brightness_ = 0.0f;        // the brightness of the clouds is stores here 
								     // and set in the pixel shader during rendering

	VERTEX* pSkyPlaneRawData_ = nullptr;
};

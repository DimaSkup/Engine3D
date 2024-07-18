#include "DDS_ImageReader.h"

#include <d3d11.h>
#include <d3dx11tex.h>
#include <string>

#include "Common/StringHelper.h"
#include "Common/log.h"
//#include "DDSTextureLoader11.h"


//using namespace DirectX;
using namespace ImgReader;


bool DDS_ImageReader::LoadTextureFromFile(const std::string & filePath,
	ID3D11Device* pDevice,
	ID3D11Resource** ppTexture,
	ID3D11ShaderResourceView** ppTextureView,
	u32& textureWidth,
	u32& textureHeight)
{
	// this function loads a DDS texture from the file by filePath
	// and initializes input parameters: texture resource, shader resource view,
	// width and height of the texture;

	assert(!filePath.empty());
	assert(pDevice != nullptr);
	assert(ppTexture != nullptr);
	assert(ppTextureView != nullptr);

	try
	{
		HRESULT hr = S_OK;
		const std::wstring wFilePath{ StringHelper::StringToWide(filePath) };

		// create a shader resource view from the texture file
		hr = D3DX11CreateShaderResourceViewFromFile(pDevice,
			wFilePath.c_str(),   // src file path
			nullptr,             // ptr load info
			nullptr,             // ptr pump
			ppTextureView,       // pp shader resource view
			nullptr);            // pHresult
		ASSERT_NOT_FAILED(hr, "can't load a DDS texture: " + filePath);

		// initialize a texture resource using the shader resource view
		(*ppTextureView)->GetResource(ppTexture);


		// load information about the texture
		D3DX11_IMAGE_INFO imageInfo;
		hr = D3DX11GetImageInfoFromFile(wFilePath.c_str(), nullptr, &imageInfo, nullptr);
		ASSERT_NOT_FAILED(hr, "can't read the image info from file: " + filePath);

		// initializa the texture width and height values
		textureWidth = imageInfo.Width;
		textureHeight = imageInfo.Height;
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't load texture from file: " + filePath);
	}

	return true;

}
////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     textclass.h
// Description:  handles all the 2D text drawing that the application
//               will need. It renders 2D text to the screen.
//               It uses FontClass to create the vertex buffer for strings
//               and then uses FontShaderClass to render this buffer;
//
// Revising:     04.06.22
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "fontclass.h"              // text font

#include "../../EffectsAndShaders/fontshaderclass.h"
#include "../../Engine/Log.h"
#include "../../Engine/SystemState.h"

#include "../../GameObjects/Vertex.h"
#include "../../GameObjects/VertexBuffer.h"
#include "../../GameObjects/IndexBuffer.h"

#include <memory>
#include <DirectXMath.h>


// Forward declaration
//
// Stores one frame transient data. This is intermediate data used by the update
// pipeline every frame and discarded at the end of the frame.
// Note that this struct is defined in the .cpp file because only
// the store needs to see it.
namespace TextDetails
{
	struct VerticesArr
	{
		std::vector<VERTEX_FONT> vertices_;
	};

	struct TextStoreTransientData 
	{
		std::vector<UINT> stringsToUpdate_;
		std::vector<POINT> positionsToUpdate_;
		std::vector<VerticesArr> verticesToUpdate_;

		void Clear()
		{
			stringsToUpdate_.clear();
			positionsToUpdate_.clear();
			verticesToUpdate_.clear();
		}
	};
}


// Stores all text related data
class TextStore final
{
public:
	TextStore();
	TextStore(const TextStore & obj) {};
	~TextStore();

	// public modification API
	void CreateSentence(ID3D11Device* pDevice,
		FontClass & font,                      // font for the text
		const UINT maxStrSize,                 // maximal length for this string
		const std::string & textContent,       // the content of the text
		const std::string & textID,
		const POINT & drawAt);                 // upper left position of the text in the window
		
	// public rendering API
	void Render(ID3D11DeviceContext* pDeviceContext,
		FontShaderClass* pFontShader,
		ID3D11ShaderResourceView* const* ppFontTexture,
		const DirectX::XMMATRIX & WVO,
		const DirectX::XMFLOAT3 & color);

	// public update API
	void Update(ID3D11DeviceContext* pDeviceContext,
		FontClass & font,
		const std::vector<std::string> & textIDsOfStringsToUpdate,
		const std::vector<std::string> & textContentToUpdate);

private:  // restrict a copying of this class instance
	TextStore & operator=(const TextStore & obj);

private:
	void BuildTextMeshes(ID3D11Device* pDevice,
		const UINT maxStrSize,                          // maximal size for this string (if it will be bigger we will have a vertex buffer overflow)
		const std::string & textContent,               
		const POINT & drawAt,                           // upper left position of the str
		FontClass & font,                               // font for the text
		std::vector<VERTEX_FONT>& vertices,
		std::vector<UINT>& indices);
	
	void RenderSentence(ID3D11DeviceContext* pDeviceContext,
		FontShaderClass* pFontShader,
		ID3D11ShaderResourceView* const* ppFontTexture,
		const DirectX::XMMATRIX & WVO,                  // world * basic_view * ortho
		const DirectX::XMFLOAT3 & textColor,
		const std::vector<VertexBuffer<VERTEX_FONT>> & vertexBuffers,
		const std::vector<IndexBuffer> & indexBuffer);


private:
	std::vector<POINT>                         positions_;                    // pos on the screen to draw at 
	std::vector<UINT>                          maxVerticesCountsPerString_;   // maximal number of vertices per each string
	std::vector<std::string>                   keys_;                         // keys to navigation
	std::vector<std::string>                   textContent_;

	std::vector<VertexBuffer<VERTEX_FONT>>     vertexBuffers_;
	std::vector<IndexBuffer>                   indexBuffers_;

	std::unique_ptr<TextDetails::TextStoreTransientData> pDataToUpdate_;
};
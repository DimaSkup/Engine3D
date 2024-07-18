////////////////////////////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
// Revising: 04.07.22
////////////////////////////////////////////////////////////////////////////////////////////
#include "TextStore.h"
#include "TextStoreUpdateHelpers.h"
#include <algorithm>



////////////////////////////////////////////////////////////////////////////////////////////////
//                          PRIVATE UPDATE API PROTOTYPES
////////////////////////////////////////////////////////////////////////////////////////////////

TextStore::TextStore() 
	: pDataToUpdate_(std::make_unique<TextDetails::TextStoreTransientData>())
{
}

TextStore::~TextStore() 
{
	Log::Debug(LOG_MACRO); 
}



////////////////////////////////////////////////////////////////////////////////////////////
//                             PUBLIC MODIFICATION API
////////////////////////////////////////////////////////////////////////////////////////////


void TextStore::CreateSentence(ID3D11Device* pDevice,
	FontClass & font,                      // font for the text
	const UINT maxStrSize,                 // maximal length for this string
	const std::string & textContent,       // the content of the text
	const std::string & textID,            // text id for this string
	const POINT & drawAt)                  // upper left position of the text in the window
{
	// check input params
	assert(pDevice != nullptr);
	assert(maxStrSize > 0);
	assert(textContent.length() > 0);
	assert(textID.length() > 0);

	try
	{
		const UINT verticesCountPerSymbol = 4;

	
		positions_.push_back(drawAt);                                                // upper left rendering position
		maxVerticesCountsPerString_.push_back(maxStrSize * verticesCountPerSymbol);  // set maximal number of vertices for this string
		keys_.push_back(textID);                                                     // an ID which is used for associative navigation
		textContent_.push_back(textContent);

		std::vector<VERTEX_FONT> vertices;
		std::vector<UINT> indices;
			
		
		BuildTextMeshes(pDevice,
			maxStrSize,
			textContent,
			drawAt,
			font, 
			vertices,
			indices);

		// initialize the vertex and index buffers for this text string
		vertexBuffers_.emplace_back(pDevice, vertices, true);
		indexBuffers_.emplace_back(pDevice, indices);
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		ASSERT_TRUE(false, "can't initialize text class obj with the text: " + textContent);
	}
}

///////////////////////////////////////////////////////////

void TextStore::Render(ID3D11DeviceContext* pDeviceContext,
	FontShaderClass* pFontShader,
	ID3D11ShaderResourceView* const* ppFontTexture,
	const DirectX::XMMATRIX & WVO,
	const DirectX::XMFLOAT3 & color)
{
	// this function renders the sentence onto the screen

	try
	{
		// render the sentence
		RenderSentence(pDeviceContext,
			pFontShader,
			ppFontTexture,
			WVO,
			color,
			{ vertexBuffers_ },
			{ indexBuffers_ });
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		ASSERT_TRUE(false, "can't render the sentence");
	}
	
	return;
}

///////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////
//                               PUBLIC UPDATE API
////////////////////////////////////////////////////////////////////////////////////////////////

void TextStore::Update(ID3D11DeviceContext* pDeviceContext,
	FontClass & font,
	const std::vector<std::string> & keysOfStringsToUpdate,
	const std::vector<std::string> & textContentToUpdate)

{
	// Update() changes the contents of the dynamic vertex buffer for the input text.

	assert(keysOfStringsToUpdate.size() <= textContent_.size());
	assert(keysOfStringsToUpdate.size() == textContentToUpdate.size());

	try
	{
		// get an array of indices to strings to update
		// also we update here text content of strings which are chosen for updating
		SelectStringsAndUpdateTextContent(
			keys_,
			textContentToUpdate,
			keysOfStringsToUpdate,
			textContent_,
			pDataToUpdate_->stringsToUpdate_);

		// if we haven't any strings to update for this frame ...
		if (pDataToUpdate_->stringsToUpdate_.size() == 0)
		{
			// ... clear the transient data and go out from the function
			pDataToUpdate_->Clear();
			return;
		}

		// allocate memory for vertices arrays
		PrepareMemoryForNewVerticesToUpdate(
			pDataToUpdate_->stringsToUpdate_,
			maxVerticesCountsPerString_, 
			pDataToUpdate_->verticesToUpdate_);

		// fill in vertices arrays with new data
		PrepareNewVerticesToUpdate(font,
			pDataToUpdate_->stringsToUpdate_,
			textContent_,
			positions_,
			pDataToUpdate_->verticesToUpdate_);

		// update vertex buffer with new vertices data
		UpdateVerticesBuffers(pDeviceContext,
			pDataToUpdate_->stringsToUpdate_,
			pDataToUpdate_->verticesToUpdate_,
			vertexBuffers_);

		pDataToUpdate_->Clear();
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "failed to update the text vertex buffer with new data");
		ASSERT_TRUE(false, "can't update the sentence");
	}

	return;

}





////////////////////////////////////////////////////////////////////////////////////////////
//                            PRIVATE MODICATION API 
////////////////////////////////////////////////////////////////////////////////////////////


void TextStore::BuildTextMeshes(
	ID3D11Device* pDevice,
	const UINT maxStrSize,
	const std::string & textContent,
	const POINT & drawAt,
	FontClass & font,                      // font for the text
	std::vector<VERTEX_FONT>& vertices,
	std::vector<UINT>& indices)
{ 
	// THIS FUNC builds a vertex and index buffer for the input string by its 
	// textContent and places its vertices at the drawAt position;

	assert(!textContent.empty());
	assert(maxStrSize >= textContent.size());

	try
	{
		const UINT verticesCountInSymbol = 4;
		const UINT indicesCountInSymbol = 6;

		vertices.resize(maxStrSize * verticesCountInSymbol);

		// fill in vertex and index arrays with initial data
		font.BuildVertexArray(vertices, textContent, drawAt);
		font.BuildIndexArray(maxStrSize * indicesCountInSymbol, indices);
	}
	catch (EngineException & e)
	{
		Log::Error(e);
		THROW_ERROR("can't build buffers for the sentence: " + textContent);
	}
}



////////////////////////////////////////////////////////////////////////////////////////////
//                               PRIVATE RENDERING API 
////////////////////////////////////////////////////////////////////////////////////////////

void TextStore::RenderSentence(ID3D11DeviceContext* pDeviceContext, 
	FontShaderClass* pFontShader,
	ID3D11ShaderResourceView* const* ppFontTexture,
	const DirectX::XMMATRIX & WVO,                   // world * basic_view * ortho
	const DirectX::XMFLOAT3 & textColor,
	const std::vector<VertexBuffer<VERTEX_FONT>> & vertexBuffers,
	const std::vector<IndexBuffer> & indexBuffers)
{
	// This function puts the sentence vertex and index buffer on the input assembler and
	// then calls the FontShaderClass object to draw the sentence that was given as input
	// to this function.

	try
	{
		UINT offset = 0;

		// set the primitive topology for all the sentences
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// set up parameters for the vertex and pixel shaders
		pFontShader->SetWorldViewOrtho(pDeviceContext, WVO);
		pFontShader->SetFontColor(pDeviceContext, textColor);
		pFontShader->SetFontTexture(pDeviceContext, ppFontTexture);

		// prepare IA stage for rendering
		pFontShader->Prepare(pDeviceContext);

		// render each text string onto the screen
		for (UINT str_idx = 0; str_idx < textContent_.size(); ++str_idx)
		{

			// set the vertices and indices buffers as active
			pDeviceContext->IASetVertexBuffers(0, 1,
				vertexBuffers[str_idx].GetAddressOf(),
				vertexBuffers[str_idx].GetAddressOfStride(),
				&offset);

			pDeviceContext->IASetIndexBuffer(indexBuffers[str_idx].Get(), DXGI_FORMAT_R32_UINT, 0);

			// render the sentence using the FontShaderClass and HLSL shaders
			pFontShader->Render(pDeviceContext, indexBuffers[str_idx].GetIndexCount());

		} // end for
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		THROW_ERROR("can't render the sentence");
	}
}

///////////////////////////////////////////////////////////




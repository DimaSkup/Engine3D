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

		IDs_.push_back((UINT)IDs_.size());
		positions_.push_back(drawAt);                                                // upper left rendering position
		maxVerticesCountsPerString_.push_back(maxStrSize * verticesCountPerSymbol);  // set maximal number of vertices for this string
		
		strings_.push_back(textContent);
		textIDs_.push_back(textID);                                                  // an ID which is used for associative navigation
		vertexBuffers_.push_back({});           
		indexBuffers_.push_back({});

		++numOfTextStrings_;


		// initialize the vertext and index buffers for this text
		this->BuildBuffers(pDevice,
			IDs_.back(),
			maxStrSize,
			textContent,
			drawAt,
			font, 
			vertexBuffers_.back(),
			indexBuffers_.back());
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't initialize text class obj with the text: " + textContent);
	}

	return;

} // end Initialize

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
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't render the sentence");
	}
	
	return;
}

///////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////
//                               PUBLIC UPDATE API
////////////////////////////////////////////////////////////////////////////////////////////////

void TextStore::Update(ID3D11DeviceContext* pDeviceContext,
	FontClass & font,
	const std::vector<std::string> & textIDsOfStringsToUpdate,
	const std::vector<std::string> & textContentToUpdate)

{
	// Update() changes the contents of the dynamic vertex buffer for the input text.

	assert(textIDsOfStringsToUpdate.size() <= numOfTextStrings_);
	assert(textIDsOfStringsToUpdate.size() == textContentToUpdate.size());

	try
	{
		// get an array of indices to strings to update
		// also we update here text content of strings which are chosen for updating
		SelectStringsAndUpdateTextContent(
			textIDs_,
			textContentToUpdate,
			textIDsOfStringsToUpdate,
			strings_,
			pDataToUpdate_->stringsToUpdate_);

		// if we haven't any strings to update for this frame ...
		if (pDataToUpdate_->stringsToUpdate_.size() == 0)
		{
			// ... clear the transient data and go out from the function
			pDataToUpdate_->Clear();
			return;
		}

#if 0
		UpdateTextContentOfStrings(
			pDataToUpdate_->stringsToUpdate_,
			textContentToUpdate,
			strings_);
#endif
		// allocate memory for vertices arrays
		PrepareMemoryForNewVerticesToUpdate(
			pDataToUpdate_->stringsToUpdate_,
			maxVerticesCountsPerString_, 
			pDataToUpdate_->verticesToUpdate_);

		// fill in vertices arrays with new data
		PrepareNewVerticesToUpdate(font,
			pDataToUpdate_->stringsToUpdate_,
			strings_,
			positions_,
			pDataToUpdate_->verticesToUpdate_);

		// update vertex buffer with new vertices data
		UpdateVerticesBuffers(pDeviceContext,
			pDataToUpdate_->stringsToUpdate_,
			pDataToUpdate_->verticesToUpdate_,
			vertexBuffers_);

		pDataToUpdate_->Clear();
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "failed to update the text vertex buffer with new data");
		COM_ERROR_IF_FALSE(false, "can't update the sentence");
	}

	return;

}





////////////////////////////////////////////////////////////////////////////////////////////
//                               PRIVATE MODICATION API 
////////////////////////////////////////////////////////////////////////////////////////////


void TextStore::BuildBuffers(ID3D11Device* pDevice,
	const UINT index,
	const UINT maxStrSize,
	const std::string & textContent,
	const POINT & drawAt,
	FontClass & font,                      // font for the text
	VertexBuffer<VERTEX_FONT> & vertexBuffer,
	IndexBuffer & indexBuffer)
{ 
	// THIS FUNC builds a vertex and index buffer for the input string by its 
	// textContent and places its vertices at the drawAt position;

	assert(!textContent.empty());
	assert(maxStrSize >= textContent.size());

	try
	{
		const UINT verticesCountInSymbol = 4;
		const UINT indicesCountInSymbol = 6;

		// create empty vertex and index arrays
		std::vector<VERTEX_FONT> verticesArr(maxStrSize * verticesCountInSymbol);
		std::vector<UINT> indicesArr;

		// fill in vertex and index arrays with initial data
		font.BuildVertexArray(verticesArr, textContent, drawAt);
		font.BuildIndexArray(maxStrSize * indicesCountInSymbol, indicesArr);

		// initialize the vertex and index buffers
		vertexBuffer.Initialize(pDevice, "text_string", verticesArr, true);
		indexBuffer.Initialize(pDevice, indicesArr);
	}
	catch (COMException & e)
	{
		Log::Error(e);
		COM_ERROR_IF_FALSE(false, "can't build buffers for the sentence: " + textContent);
	}

	return;
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
		pFontShader->SetShaderParameters(pDeviceContext, WVO, textColor, ppFontTexture);

		// render each text string onto the screen
		for (UINT str_idx = 0; str_idx < numOfTextStrings_; ++str_idx)
		{
			const VertexBufferStorage::VertexBufferData & vertexBuffData = vertexBuffers[str_idx].GetData();
			const IndexBufferStorage::IndexBufferData & indexBuffData = indexBuffers[str_idx].GetData();

			// set the vertices and indices buffers as active
			pDeviceContext->IASetVertexBuffers(0, 1,
				&vertexBuffData.pBuffer_,
				&vertexBuffData.stride_,
				&offset);

			pDeviceContext->IASetIndexBuffer(indexBuffData.pBuffer_, DXGI_FORMAT_R32_UINT, 0);

			// render the sentence using the FontShaderClass and HLSL shaders
			pFontShader->Render(pDeviceContext,	indexBuffData.indexCount_);

		} // end for
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't render the sentence");
	}

	return;
}

///////////////////////////////////////////////////////////




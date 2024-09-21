////////////////////////////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
// Revising: 04.07.22
////////////////////////////////////////////////////////////////////////////////////////////
#include "TextStore.h"
#include "TextStoreUpdateHelpers.h"
#include <algorithm>

#include "../../Engine/Log.h"


////////////////////////////////////////////////////////////////////////////////////////////////
//                          PRIVATE UPDATE API PROTOTYPES
////////////////////////////////////////////////////////////////////////////////////////////////

TextStore::TextStore() 
	: pDataToUpdate_(std::make_unique<TextDetails::TextStoreTransientData>())
{
}

TextStore::~TextStore() 
{
	Log::Debug(); 
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
		throw EngineException("can't initialize text class obj with the text: " + textContent);
	}
}

///////////////////////////////////////////////////////////

void TextStore::GetRenderingData(
	std::vector<ID3D11Buffer*>& outVbPtrs,
	std::vector<ID3D11Buffer*>& outIbPtrs,
	std::vector<u32>& outIndexCounts)

{
	try
	{
		const size numBuffers = std::ssize(outVbPtrs);

		outVbPtrs.reserve(numBuffers);
		outIbPtrs.reserve(numBuffers);
		outIndexCounts.reserve(numBuffers);

		for (VertexBuffer<VERTEX_FONT>& vb : vertexBuffers_)
			outVbPtrs.push_back(vb.Get());

		for (IndexBuffer& ib : indexBuffers_)
			outIbPtrs.push_back(ib.Get());

		for (IndexBuffer& ib : indexBuffers_)
			outIndexCounts.push_back(ib.GetIndexCount());


	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		throw EngineException("can't render the sentence");
	}
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
		Log::Error("failed to update the text vertex buffer with new data");
		throw EngineException("can't update the sentence");
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
		throw EngineException("can't build buffers for the sentence: " + textContent);
	}
}
#pragma once




////////////////////////////////////////////////////////////////////////////////////////////////
//                             PRIVATE UPDATE API
////////////////////////////////////////////////////////////////////////////////////////////////

const ptrdiff_t GetPosOfKey(
	const std::vector<std::string>& keysArr,
	const std::string& searchedKey)
{
	// find position of the searched text key in the keys array

	const auto keyIter = std::find(keysArr.begin(), keysArr.end(), searchedKey);
	//Assert::True(keyIter != keysArr.end(), "there is no such a key: " + searchedKey);
	return std::distance(keysArr.begin(), keyIter); 
}

///////////////////////////////////////////////////////////

void SelectStringsAndUpdateTextContent(
	const std::vector<std::string> & originKeysToText,
	const std::vector<std::string> & newTextContents,
	const std::vector<std::string> & keysOfStringsToUpdate,
	_Inout_ std::vector<std::string> & originTextContents,
	_Out_   std::vector<UINT> & outStringsIdxsToUpdate)
{
	// select indices of strings which will be updated
	// and update text content of these strings with the new input ones

	size_t idx = 0;
	for (const std::string& newText : newTextContents)
	{
		const ptrdiff_t str_idx = GetPosOfKey(originKeysToText, keysOfStringsToUpdate[idx++]);

		// if the origin string and new string aren't the same
		// we update the text content and store the pos in arr of this text string
		if (originTextContents[str_idx] != newText)
		{
			originTextContents[str_idx] = newText;
			outStringsIdxsToUpdate.push_back((UINT)str_idx);
		}
	}
}

///////////////////////////////////////////////////////////

void PrepareMemoryForNewVerticesToUpdate(
	const std::vector<UINT>& stringsIdxsToUpdate,
	const std::vector<UINT>& maxVerticesCountsPerBuffer,
	_Inout_ std::vector<TextDetails::VerticesArr>& newVertices)
{
	UINT index = 0;

	// allocate memory for an array of the vertices arrays
	newVertices.resize(stringsIdxsToUpdate.size());

	for (const UINT str_idx : stringsIdxsToUpdate)
	{
		// allocate memory for necessary count of vertices for the string to update
		newVertices[index++].vertices_.resize(maxVerticesCountsPerBuffer[str_idx]);
	}
}

///////////////////////////////////////////////////////////

void PrepareNewVerticesToUpdate(FontClass& font,
	const std::vector<UINT>& stringsIdxsToUpdate,
	const std::vector<std::string>& textContent,
	const std::vector<POINT>& drawAtPositions,
	_Inout_ std::vector<TextDetails::VerticesArr>& tempVerticesArr)
{
	// rebuild vertices arrays for vertex buffers 
	//of the strings which will be updated

	for (size_t idx = 0; idx < stringsIdxsToUpdate.size(); ++idx)
	{
		const UINT str_idx = stringsIdxsToUpdate[idx];

		font.BuildVertexArray(
			tempVerticesArr[idx].vertices_,    // write here new raw data of vertices 
			textContent[str_idx],             
			drawAtPositions[str_idx]);         // upper left position to draw at
	}
}

///////////////////////////////////////////////////////////

void UpdateVerticesBuffers(ID3D11DeviceContext* pDeviceContext,
	const std::vector<UINT>& stringsIdxsToUpdate,
	const std::vector<TextDetails::VerticesArr>& newVertices,
	_Inout_ std::vector<VertexBuffer<VertexFont>>& vertexBuffers)
{
	// update the sentences vertex buffers with new vertices data

	UINT data_idx = 0;

	for (const UINT str_idx : stringsIdxsToUpdate)
	{
		vertexBuffers[str_idx].UpdateDynamic(pDeviceContext, newVertices[data_idx].vertices_);
		++data_idx;
	}
}


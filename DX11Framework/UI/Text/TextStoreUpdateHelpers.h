#pragma once




////////////////////////////////////////////////////////////////////////////////////////////////
//                             PRIVATE UPDATE API
////////////////////////////////////////////////////////////////////////////////////////////////

const ptrdiff_t GetPositionOfStringInArr(
	const std::vector<std::string> & stringsArr,
	const std::string& str)
{
	// find position of this string in the array
	const ptrdiff_t str_idx = std::distance(
		stringsArr.begin(),                                    // from
		std::find(stringsArr.begin(), stringsArr.end(), str)); // to

	assert(((size_t)str_idx < stringsArr.size()) && "there is no such a string is the array");

	return str_idx;
}

///////////////////////////////////////////////////////////

void SelectStringsAndUpdateTextContent(
	const std::vector<std::string> & originTextIDs,
	const std::vector<std::string> & newTextContents,
	const std::vector<std::string> & textIDsOfStringsToUpdate,
	_Inout_ std::vector<std::string> & originTextContents,
	_Out_   std::vector<UINT> & outStringsToUpdate)
{
	//
	// select indices of strings which will be updates
	//

	for (size_t idx = 0; idx < newTextContents.size(); ++idx)
	{
		const ptrdiff_t str_idx = GetPositionOfStringInArr(originTextIDs, textIDsOfStringsToUpdate[idx]);

		// if the origin string and new string aren't the same
		if (originTextContents[str_idx] != newTextContents[idx])
		{
			// update text content
			originTextContents[str_idx] = newTextContents[idx];

			// store the position of this string 
			outStringsToUpdate.push_back((UINT)str_idx);
		}
	}
}

///////////////////////////////////////////////////////////

#if 0
void UpdateTextContentOfStrings(
	const std::vector<UINT>& inStringsToUpdate,
	const std::vector<std::string>& newTextContents,
	_Inout_ std::vector<std::string>& stringsToUpdate)
{
	// Update text content of strings by particular indices

	UINT idx = 0;

	for (const UINT str_idx : inStringsToUpdate)
	{
		stringsToUpdate[str_idx] = newTextContents[idx++];
	}
}
#endif

///////////////////////////////////////////////////////////

void PrepareMemoryForNewVerticesToUpdate(
	const std::vector<UINT>& stringsToUpdate,
	const std::vector<UINT>& verticesCountsPerBufferToUpdate,
	_Inout_ std::vector<TextDetails::TemporalVerticesToUpdate>& tempVerticesArr)
{
	UINT index = 0;

	// allocate memory for an array of the vertices arrays
	tempVerticesArr.resize(stringsToUpdate.size());

	for (const UINT str_idx : stringsToUpdate)
	{
		// allocate memory for necessary count of vertices for the string to update
		tempVerticesArr[index].vertices_.resize(verticesCountsPerBufferToUpdate[str_idx]);
		++index;
	}
}

///////////////////////////////////////////////////////////

void PrepareNewVerticesToUpdate(FontClass& font,
	const std::vector<UINT>& stringsToUpdate,
	const std::vector<std::string>& textData,
	const std::vector<POINT>& drawAtPositions,
	_Inout_ std::vector<TextDetails::TemporalVerticesToUpdate>& tempVerticesArr)
{
	//
	// rebuild vertices arrays for vertex buffers of the strings which will be updated
	//

	UINT index = 0;

	for (const UINT str_idx : stringsToUpdate)
	{
		// build the vertices array
		font.BuildVertexArray(
			tempVerticesArr[index].vertices_,    // write here new raw data of vertices 
			textData[str_idx],                   // text content
			drawAtPositions[str_idx]);           // upper left position

		++index;
	}
}

///////////////////////////////////////////////////////////

void UpdateVerticesBuffers(ID3D11DeviceContext* pDeviceContext,
	const std::vector<UINT>& stringsToUpdate,
	const std::vector<TextDetails::TemporalVerticesToUpdate>& tempVerticesArr,
	_Inout_ std::vector<VertexBuffer<VERTEX_FONT>>& vertexBuffers)
{
	// update the sentences vertex buffers with new text data

	UINT data_idx = 0;

	for (const UINT str_idx : stringsToUpdate)
	{
		vertexBuffers[str_idx].UpdateDynamic(pDeviceContext, tempVerticesArr[data_idx].vertices_);
		++data_idx;
	}
}


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
//#include "SentenceType.h"           // DTO with text data
#include "../../ShaderClass/fontshaderclass.h"
#include "../../ShaderClass/DataContainerForShaders.h"
#include "../../Engine/Log.h"

#include "../../GameObjects/Vertex.h"
#include "../../GameObjects/VertexBuffer.h"
#include "../../GameObjects/IndexBuffer.h"

#include <memory>
#include <DirectXMath.h>








//////////////////////////////////
// Class name: TextClass
//////////////////////////////////
class TextClass final
{
public:
	TextClass();
	TextClass(const TextClass & obj) {};
	~TextClass();

	bool Initialize(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const int maxStrSize,                  // maximal size of the string
		FontClass* pFont,                      // font for the text
		FontShaderClass* pFontShader,          // font shader for rendering text onto the screen
		const std::string & text,              // the content of the text
		const POINT & position);               // upper left position of the text in the window

	void Render(ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX & WVO,
		const DirectX::XMFLOAT3 & color);
#if 0
		const DirectX::XMMATRIX & worldMatrix,
		const DirectX::XMMATRIX & baseViewMatrix,
		const DirectX::XMMATRIX & orthoMatrix);
#endif

	bool Update(ID3D11DeviceContext* pDeviceContext,
		_Inout_ std::vector<VERTEX_FONT> & verticesArr,  // a temporal buffer for vertices data for updating
		const std::string & newText, 
		const POINT & newPosition);

private:  // restrict a copying of this class instance
	
	TextClass & operator=(const TextClass & obj);

private:
	void BuildSentence(ID3D11Device* pDevice, 
		const UINT stringSize,
		const std::string & text,
		const POINT & position); 

	
	void UpdateSentenceVertexBuffer(ID3D11DeviceContext* pDeviceContext,
		_Inout_ std::vector<VERTEX_FONT> & verticesArr,  // a temporal buffer for vertices data for updating
		const std::string & nextText,
		const POINT & position);  
	
	bool RenderSentence(ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX & WVO,          // world * basic_view * ortho
		const DirectX::XMFLOAT3 & textColor,
		ID3D11ShaderResourceView* const* ppTexture);


private:
	VertexBuffer<VERTEX_FONT>     vertexBuffer_;  // a vertex buffer specially for text data
	IndexBuffer                   indexBuffer_;
	FontClass*        pFont_ = nullptr;
	FontShaderClass*  pFontShader_ = nullptr;
};
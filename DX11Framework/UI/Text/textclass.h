////////////////////////////////////////////////////////////////////
// Filename:     textclass.h
// Description:  handles all the 2D text drawing that the application
//               will need. It renders 2D text to the screen.
//               It uses FontClass to create the vertex buffer for strings
//               and then uses FontShaderClass to render this buffer;
//
// Revising:     04.06.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "fontclass.h"              // text font
#include "SentenceType.h"           // DTO with text data
#include "../../ShaderClass/fontshaderclass.h"
#include "../../Engine/Log.h"

#include "../../Model/Vertex.h"
#include "../../Model/VertexBuffer.h"
#include "../../Model/IndexBuffer.h"

#include <memory>
#include <DirectXMath.h>






//////////////////////////////////
// Class name: TextClass
//////////////////////////////////
class TextClass final
{
public:
	TextClass(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	~TextClass();

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,
		int screenWidth, int screenHeight,
		int stringSize,                        // maximal size of the string
		FontClass* pFont,                      // font for the text
		FontShaderClass* pFontShader,          // font shader for rendering text onto the screen
		const char* textContent,               // the content of the text
		int posX, int posY,                    // upper left position of the text in the window
		float red, float green, float blue);   // colour of the text

	bool Render(ID3D11DeviceContext* deviceContext,
		const DirectX::XMMATRIX & worldMatrix,
		const DirectX::XMMATRIX & baseViewMatrix,
		const DirectX::XMMATRIX & orthoMatrix);

	bool Update(ID3D11DeviceContext* pDeviceContext, const std::string & newText, const DirectX::XMFLOAT2 & newPosition, const DirectX::XMFLOAT4 & newColor);

private:  // restrict a copying of this class instance
	TextClass(const TextClass & obj);
	TextClass & operator=(const TextClass & obj);

private:
	bool BuildSentence(ID3D11Device* pDevice, int stringSize,
		const char* textContent,
		int posX, int posY,
		float red, float green, float blue);  // first of all we create an empty sentence (with empty vertices data) and after we update this sentence with text data

	
	bool UpdateSentenceVertexBuffer(ID3D11DeviceContext* pDeviceContext, const std::string & nextText, int posX, int posY);
	

	bool RenderSentence(ID3D11DeviceContext* pDeviceContext, 
						const DirectX::XMMATRIX & worldMatrix,
						const DirectX::XMMATRIX & baseViewMatrix,
						const DirectX::XMMATRIX & orthoMatrix);

private: // helpers
	// checks if we must update the current sentence because of new differ params
	bool CheckSentence(SentenceType* pPrevSentence, const std::string & newText, const DirectX::XMFLOAT2 & newPosition); 
	bool CheckColor(const DirectX::XMFLOAT4 & prevColor, const DirectX::XMFLOAT4 & newColor);  // checks if both input colours are the same
private:
	FontClass* pFont_ = nullptr;
	FontShaderClass* pFontShader_ = nullptr;

	VertexBuffer<VERTEX_FONT>* pVertexBuffer_ = nullptr;
	IndexBuffer*               pIndexBuffer_ = nullptr;

	int screenWidth_ = 0;
	int screenHeight_ = 0;
	SentenceType* pSentence_ = nullptr;
};
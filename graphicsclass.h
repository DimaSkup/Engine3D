//////////////////////////////////
// Filename: graphicsclass.h
//////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

//////////////////////////////////
// GLOBALS
//////////////////////////////////
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

//////////////////////////////////
// MY CLASS INCLUDES
//////////////////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
//#include "textureshaderclass.h"
//#include "modelclass.h"
//#include "lightshaderclass.h"
//#include "lightclass.h"
//#include "bitmapclass.h"
#include "textclass.h"






//////////////////////////////////
// Class name: GraphicsClass
//////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	bool Render();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	//ModelClass* m_Model;
	//LightShaderClass* m_LightShader;
	//LightClass* m_Light;
	//TextureShaderClass* m_TextureShader;
	//BitmapClass* m_Bitmap;
	TextClass* m_Text;
};

#endif
#pragma once

/////////////////////////////
// PRE-PROCESSING DIRECTIVES
/////////////////////////////
#define WIN32_LEAN_AND_MEAN

/////////////////////////////
// INCLUDES
/////////////////////////////
#include <windows.h>
#include <string>
#include <ctime>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>



/////////////////////////////
// MY CLASS INCLUDES
/////////////////////////////
#include "macros.h"

#pragma warning (disable : 4996)
#pragma warning (disable : 4005)

/////////////////////////////
// LIBRARIES
/////////////////////////////
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

/////////////////////////////
// INCLUDES
/////////////////////////////
#include <dxgi.h>	// a DirectX graphic interface header
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx11async.h>
#include <d3dx10math.h>
#include <d3dcompiler.h>


//////////////////////////////////
// DEFINITIONS
//////////////////////////////////
#define THIS_FUNC "%s()::%d: %s",__FUNCTION__, __LINE__
#define THIS_FUNC_EMPTY "%s()::%d", __FUNCTION__, __LINE__


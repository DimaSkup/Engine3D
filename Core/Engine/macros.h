#pragma once

// operations with memory 
#define _DELETE(p)			{ if(p) {delete(p); p = nullptr;} }
#define _DELETE_ARR(p)      { if(p) {delete[](p); p = nullptr; }}
#define _SHUTDOWN(p)		{ if(p) {p->Shutdown(); delete(p); p = nullptr;} }
#define _RELEASE(p)			{ if(p) {p->Release(); p = nullptr;} }

// debug macroses
#define LOG_MACRO        __FUNCTION__, __LINE__

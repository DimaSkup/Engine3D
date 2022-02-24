#pragma once

#define _DELETE(p)			{ if(p) {delete(p); p = nullptr;} }
#define _SHUTDOWN(p)		{ if(p) {p->Shutdown(); delete(p); p = nullptr;} }

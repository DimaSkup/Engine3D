#pragma once

#define _SHUTDOWN(p)		{ if(p) {p->Shutdown(); delete(p); p = nullptr;} }

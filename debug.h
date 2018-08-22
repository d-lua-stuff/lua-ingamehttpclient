#pragma once

#include "stdafx.h"

#if defined(_DEBUG) && defined(_WIN32)
#define DEBUG_PRINT(msg) OutputDebugStringA(msg); OutputDebugStringA("\n")
#else
#define DEBUG_PRINT(msg) do {} while (false)
#endif

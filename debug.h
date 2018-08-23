#pragma once

#include "stdafx.h"

#if defined(_DEBUG) && defined(_WIN32)
#define MAX_DEBUG_MSG_LEN 2048
#define DEBUG_PRINT(msg) do { char buffer[MAX_DEBUG_MSG_LEN]; snprintf(buffer, MAX_DEBUG_MSG_LEN, "ingamehttpclient: %s\n", msg); OutputDebugStringA(buffer); } while (false)
#else
#define DEBUG_PRINT(msg) do {} while (false)
#endif

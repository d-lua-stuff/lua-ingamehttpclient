#include "stdafx.h"
#include "debug.h"

#if defined(_WIN32)

// Log DLL loading and unloading.
// Linux version would use something like __attribute__((constructor)) and __attribute__((destructor))

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			DEBUG_PRINT("ingamehttpclient: DLL_PROCESS_ATTACH");
			break;
		case DLL_THREAD_ATTACH:
			//DEBUG_PRINT("ingamehttpclient: DLL_THREAD_ATTACH");
			break;
		case DLL_THREAD_DETACH:
			//DEBUG_PRINT("ingamehttpclient: DLL_THREAD_DETACH");
			break;
		case DLL_PROCESS_DETACH:
			DEBUG_PRINT("ingamehttpclient: DLL_PROCESS_DETACH");
			break;
	}
	return TRUE;
}

#endif

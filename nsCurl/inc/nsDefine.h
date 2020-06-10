#pragma once

#define NSISFUNC(name) extern "C" void __declspec(dllexport) name(HWND hWndParent, int string_size, LPTSTR variables, stack_t** stacktop, extra_parameters* extra)

#define REGSITER_CALLBACK(INST) do \
{ \
	if(!g_bCallbackRegistred) g_bCallbackRegistred = (extra->RegisterPluginCallback((HMODULE)(INST), PluginCallback) == 0); \
	if(!g_hApcHandle) g_hApcHandle = (HANDLE)::_beginthreadex(NULL, 0, HttpAsyncProc, NULL, 0, &g_ApcId);\
} \
	while(0)

#ifdef _DEBUG
#define NSDM_DEBUG_INFO(fmt, ...) DEBUG_INFO(fmt, __VA_ARGS__)
#else
#define NSDM_DEBUG_INFO(fmt, ...) (void*)0
#endif
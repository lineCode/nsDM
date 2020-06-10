#pragma once

#define NSISFUNC(name) extern "C" void __declspec(dllexport) name(HWND hWndParent, int string_size, LPTSTR variables, stack_t** stacktop, extra_parameters* extra)

#define REGSITER_CALLBACK(INST) do \
{ \
	if(!g_bCallbackRegistred) g_bCallbackRegistred = (extra->RegisterPluginCallback((HMODULE)(INST), PluginCallback) == 0); \
} \
while(0)

#ifdef _DEBUG
#define NSDM_DEBUG_INFO(fmt, ...) DEBUG_INFO(fmt, __VA_ARGS__)
#else
#define NSDM_DEBUG_INFO(fmt, ...) (void*)0
#endif

#define MACRO_DisableMaxStyle(hwnd) ::SetWindowLongPtr(hwnd, GWL_STYLE, ::GetWindowLongPtr(hwnd, GWL_STYLE ) & (~WS_MAXIMIZEBOX) )

#define CHAIN_NSDM_EVENT(func) \
	if (DM::DMEVT_CMD == uCode && IsValidString(pEvt->m_szNameFrom))\
	{\
	iErr = func(pEvt->m_szNameFrom);\
	break;\
	}

#define CHAIN_NSDM_HANDLER(cd, func)\
	if (cd == uCode && IsValidString(pEvt->m_szNameFrom))\
	{\
	iErr = func(pEvt);\
	break;\
	}
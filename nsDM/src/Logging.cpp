#include "stdafx.h"
#include "Logging.h"

extern "C" void InnerDebug(char* pFmtStr)
{
#ifdef UNICODE
    LPWSTR pWide=NULL;
    int len;
    BOOL bret;
    len = (int) strlen(pFmtStr);
    pWide = new wchar_t[len*2];
    bret = MultiByteToWideChar(CP_ACP,NULL,pFmtStr,-1,pWide,len*2);
    if(bret) {
        OutputDebugString(pWide);
    } else {
        OutputDebugString(L"can not change fmt string");
    }
    delete [] pWide;
#else
    OutputDebugString(pFmtStr);
#endif
    return ;
}

extern "C" void DebugOutString(const char* file,int lineno,const char* fmt,...)
{
}


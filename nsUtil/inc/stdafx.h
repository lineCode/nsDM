// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>



// lib
#ifdef _DEBUG
#pragma comment(lib,"msi.lib")
#pragma comment(lib,"Dbghelp.lib")
#pragma comment(lib,"Rpcrt4.lib")
#pragma comment(lib,"mstask.lib")
#else
#pragma comment(lib,"msi.lib")
#pragma comment(lib,"Dbghelp.lib")
#pragma comment(lib,"Rpcrt4.lib")
#pragma comment(lib,"mstask.lib")
#endif

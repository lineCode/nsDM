// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>



#pragma warning(push, 0) // ���õ�������ľ���
// lib
#ifdef _DEBUG
#pragma comment(lib,"DMMain_d.lib")
// #pragma comment(lib,"zlib_d.lib")
#pragma comment(lib,"mstask.lib")
#else
#pragma comment(lib,"DMMain.lib")
// #pragma comment(lib,"zlib.lib")
#pragma comment(lib,"mstask.lib")
#endif
#include "DmMainOutput.h"
#pragma warning(pop)


#define WAITING_FOR_DEBUGGER_TO_ATTACH 0

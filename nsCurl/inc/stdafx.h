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
#pragma comment(lib,"easy_libcurl_d.lib")
#pragma comment(lib,"mstask.lib")
#else
#pragma comment(lib,"easy_libcurl.lib")
#pragma comment(lib,"mstask.lib")
#endif


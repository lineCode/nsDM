// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>



#pragma warning(push, 0) // 禁用第三方库的警告
// lib
#ifdef _DEBUG
#pragma comment(lib,"DMMain_d.lib")
#pragma comment(lib,"zlib_d.lib")
#pragma comment(lib,"mstask.lib")
#else
#pragma comment(lib,"DMMain.lib")
#pragma comment(lib,"zlib.lib")
#pragma comment(lib,"mstask.lib")
#endif
#include "DmMainOutput.h"
#pragma warning(pop)
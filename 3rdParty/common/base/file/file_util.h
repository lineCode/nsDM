/* -------------------------------------------------------------------------
//	FileName	：	file_util.h
//	Creator		：	yanbozhao
//	CreateTime	：	2015.06.01 11:06:00
//	Description	：
//
// -----------------------------------------------------------------------*/
#ifndef __BASE_FILE_UTIL_H__
#define __BASE_FILE_UTIL_H__

#include <string>
#include <windows.h>
// -------------------------------------------------------------------------

namespace base {
	namespace file_util {
		std::wstring GetModulePathDir();
		std::wstring GetMoudleFileName(LPCWSTR fileName);

		std::wstring GetUserAppDataRoot();

		bool PathExists(const std::wstring& path);
		bool DirectoryExists(const std::wstring& path);

		// 格式化目录（将目录中的“/"改为"\\"，字母均改为小写）
		void FormatDirSplit(wchar_t* dirPath);

		size_t GetFileSize(FILE* file);
		size_t GetFileSize(const std::wstring& filePath);

		std::wstring SafePathCombine(const wchar_t* p1, const wchar_t* p2);

		bool SaveFile(const wchar_t* path, const char* data, unsigned int length);
	}
}

// -------------------------------------------------------------------------


#endif /* __BASE_FILE_UTIL_H__ */
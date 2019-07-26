/* -------------------------------------------------------------------------
//	FileName	：	file_util.h
//	Creator		：	yanbozhao
//	CreateTime	：	2015.06.01 11:06:00
//	Description	：
//
// -----------------------------------------------------------------------*/

#include "file_util.h"
#include <shlwapi.h>
#include <shlobj.h>
// -------------------------------------------------------------------------

namespace base {
	namespace file_util {

		std::wstring GetModulePathDir() {
			wchar_t buffer[_MAX_PATH] = { 0 };
			::GetModuleFileName(NULL, buffer, MAX_PATH);

			const wchar_t *lastBackSlash = wcsrchr(buffer, L'\\');
			if (lastBackSlash == NULL) {
				return std::wstring();
			}
			ZeroMemory(buffer + (lastBackSlash - buffer) + 1, sizeof(wchar_t)* (wcslen(buffer) - (lastBackSlash - buffer) - 1));

			std::wstring retDir(buffer);
			unsigned int i = retDir.rfind(L"\\");
			retDir.erase(i + 1, -1);

			return retDir;
		}

		std::wstring GetMoudleFileName(LPCWSTR fileName) {
			std::wstring path(GetModulePathDir());
			path.append(fileName);
			return path;
		}

		std::wstring GetUserAppDataRoot() {
			wchar_t buffer[_MAX_PATH] = { 0 };
			SHGetSpecialFolderPath(NULL, buffer, CSIDL_APPDATA, NULL);
			return std::wstring(buffer);
		}

		bool PathExists(const std::wstring& path) {
			return (GetFileAttributes(path.c_str()) != INVALID_FILE_ATTRIBUTES);
		}

		bool DirectoryExists(const std::wstring& path) {
			DWORD fileattr = GetFileAttributes(path.c_str());
			if (fileattr != INVALID_FILE_ATTRIBUTES) {
				return (fileattr & FILE_ATTRIBUTE_DIRECTORY) != 0;
			}
			return false;
		}

		void FormatDirSplit(wchar_t* dirPath) {
			bool bPrevIsSplit = false;
			for (; *dirPath != L'\0'; ++dirPath) {
				if (*dirPath == L'/' || *dirPath == L'\\') {
					if (bPrevIsSplit) {
						wchar_t* tmpfilePath = dirPath--;	//当期位置需要重新计算
						while (true) {
							*tmpfilePath = *(tmpfilePath + 1);
							if (*tmpfilePath == L'\0')
								break;
							++tmpfilePath;
						}
					}
					else {
						*dirPath = L'\\';
						bPrevIsSplit = true;
					}
				}
				else {
					if (*dirPath <= 'Z' && *dirPath >= 'A') {
						*dirPath = (*dirPath - ('Z'-'z'));
					}
					bPrevIsSplit = false;
				}
			}
		}

		size_t GetFileSize(FILE* file) {
			if (NULL == file) {
				return 0;
			}

			long curr = ::ftell(file);       // remember where we are
			if (curr < 0) {
				return 0;
			}

			::fseek(file, 0, SEEK_END);         // go to the end
			long size = ::ftell(file);			 // record the size
			if (size < 0) {
				size = 0;
			}
			::fseek(file, curr, SEEK_SET);        // go back to our prev loc
			return size;
		}

		size_t GetFileSize(const std::wstring& filePath) {
			FILE* pFile = NULL;
			pFile = _wfsopen(filePath.c_str(), L"rb", _SH_DENYWR);
			if (!pFile) {
				return 0;
			}
			return GetFileSize(pFile);
		}

		std::wstring SafePathCombine(const wchar_t* p1, const wchar_t* p2)
		{
			wchar_t ocx_path[MAX_PATH] = {0};
			::PathCombine(ocx_path, p1, p2);
			return ocx_path;
		}

		bool SaveFile(const wchar_t* path, const char* data, unsigned int length)
		{
			FILE* pFile = NULL;
			_wfopen_s(&pFile, path, L"wb");
			if (!pFile) {
				return false;
			}
			fwrite(data, 1, length, pFile);
			fclose(pFile);
			return true;
		}

	}
}

// -------------------------------------------------------------------------

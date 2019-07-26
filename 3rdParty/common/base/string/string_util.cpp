/* -------------------------------------------------------------------------
//	FileName	：	string_util.cpp
//	Creator		：	yanbozhao
//	CreateTime	：	2015.05.29 11:06:00
//	Description	：
//
// -----------------------------------------------------------------------*/

#include "string_util.h"

#include <cassert>
// -------------------------------------------------------------------------

namespace base {
	namespace string_util {

		/*===================================================//
			 以下是处理字符串两边的空白部分（Trim）的实现
		//===================================================*/

		const char	  kWhitespaceASCII[] = " ";
		const wchar_t kWhitespaceWide[] = L" ";

		TrimPositions TrimWhitespace(const std::wstring& input, TrimPositions positions, std::wstring* output) {
			return TrimStringT(input, kWhitespaceWide, positions, output);
		}

		TrimPositions TrimWhitespaceASCII(const std::string& input, TrimPositions positions, std::string* output) {
			return TrimStringT(input, kWhitespaceASCII, positions, output);
		}

		// -------------------------------------------------------------------------

		/*==============================================//
				以下是处理字符串编码转换的实现
		//==============================================*/

		std::wstring ToStringWide(const char* c, size_t len, UINT codePage /*= CP_ACP*/) {
			std::wstring buf;
			if (NULL == c || (len < 0 && len != -1)) {
				return buf;
			}

			int nChars = MultiByteToWideChar(codePage, 0, c, len, NULL, 0);
			if (len == -1) {
				--nChars;
			}
			if (nChars == 0) {
				return L"";
			}

			buf.resize(nChars);
			MultiByteToWideChar(codePage, 0, c, len, const_cast<wchar_t*>(buf.c_str()), nChars);

			return buf;
		}

		std::wstring ToStringWide(const std::string& strASCII, UINT codePage /*= CP_ACP*/) {
			return ToStringWide(strASCII.c_str(), strASCII.length(), codePage);
		}

		std::string ToStringASCII(const wchar_t* c, int len, UINT codePage /*= CP_ACP*/) {
			std::string buf;
			if (NULL == c || (len < 0 && len != -1)) {
				return buf;
			}

			int nChars = WideCharToMultiByte(codePage, 0, c, len, NULL, 0, NULL, NULL);
			if (len == -1) {
				--nChars;
			}
			if (nChars == 0) {
				return "";
			}

			buf.resize(nChars);
			WideCharToMultiByte(codePage, 0, c, len, const_cast<char*>(buf.c_str()), nChars, NULL, NULL);

			return buf;
		}

		std::string ToStringASCII(const std::wstring& strWide, UINT codePage /*= CP_ACP*/) {
			return ToStringASCII(strWide.c_str(), strWide.length(), codePage);
		}


		// -------------------------------------------------------------------------
		
		/*=========================================//
				以下方法主要处理数字和字符串转化
		//=========================================*/

		std::string IntToStringASCII(int num) {
			char buffer[MAX_PATH] = { 0 };
			_itoa_s(num, buffer, MAX_PATH, 10);
			return std::string(buffer, strnlen_s(buffer, MAX_PATH));
		}

		std::wstring IntToStringWide(int num) {
			wchar_t buffer[MAX_PATH] = { 0 };
			_itow_s(num, buffer, MAX_PATH, 10);
			return std::wstring(buffer, wcsnlen_s(buffer, MAX_PATH));
		}

		int ASCIIStringToInt(const std::string& str) {
			return atoi(str.c_str());
		}

		int WideStringToInt(const std::wstring& strW) {
			return _wtoi(strW.c_str());
		}

		// -------------------------------------------------------------------------

		/*===================================================//
					以下方法主要处理字符串分割
	    //===================================================*/
		std::vector<std::wstring>& SplitWide(wchar_t* source, const wchar_t* token, std::vector<std::wstring>& dsts) {
			wchar_t* next_contens = NULL;

			wchar_t* contents = wcstok_s(source, token, &next_contens);
			while (contents)
			{
				dsts.push_back(contents);
				contents = wcstok_s(NULL, token, &next_contens);
			}
			return dsts;
		}

		std::vector<std::string>& SplitASCII(char* source, const char* token, std::vector<std::string>& dsts) {
			char* next_contens = NULL;

			char* contents = strtok_s(source, token, &next_contens);
			while (contents)
			{
				dsts.push_back(contents);
				contents = strtok_s(NULL, token, &next_contens);
			}
			return dsts;
		}

		int versioncmp(char const* v1, char const* v2)
		{
			std::vector<std::string> t1, t2;
			char buf[128] = {0};
			assert(_countof(buf) > strlen(v1));
			assert(_countof(buf) > strlen(v2));
	
			strcpy_s(buf, v1);
			base::string_util::SplitASCII(buf, ".", t1);
	
			strcpy_s(buf, v2);
			base::string_util::SplitASCII(buf, ".", t2);
	
			size_t loop_count = max(t1.size(), t2.size());
			t1.resize(loop_count, "0");
			t2.resize(loop_count, "0");
			for (size_t i = 0; i < loop_count; ++i)
			{
				long b1 = strtol(t1[i].c_str(), NULL, 10);
				long b2 = strtol(t2[i].c_str(), NULL, 10);
				if (b1 > b2)
				{
					return 1;
				}
				else if (b2 > b1)
				{
					return -1;
				}
			}
			return 0;
		}

	}
}

// -------------------------------------------------------------------------

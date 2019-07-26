/* -------------------------------------------------------------------------
//	FileName	：	string_util.h
//	Creator		：	yanbozhao
//	CreateTime	：	2015.05.29 11:06:00
//	Description	：
//
// -----------------------------------------------------------------------*/
#ifndef __BASE_STRING_UTIL_H__
#define __BASE_STRING_UTIL_H__

#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <locale>
// -------------------------------------------------------------------------

namespace base {
	namespace string_util {

		/*===================================================//
				以下方法主要处理字符和字符串的大小写转换
		//===================================================*/

		template<typename charT>
		inline charT ToLowerASCII(charT c) {
			return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
		}

		template<typename charT>
		inline charT ToUpperASCII(charT c) {
			return (c >= 'a' && c <= 'z') ? (c + ('A' - 'a')) : c;
		}

		template <typename strT>
		inline void StringToLowerASCII(strT* s) {
			typename strT::iterator it = s->begin();
			for (it; it != s->end(); ++it) {
				*it = ToLowerASCII(*it);
			}
		}

		template <typename strT>
		inline strT StringToLowerASCII(const strT& s) {
			strT output(s);
			StringToLowerASCII(&output);
			return output;
		}

		template <typename strT>
		inline void StringToUpperASCII(strT* s) {
			typename strT::iterator it = s->begin();
			for (it; it != s->end(); ++it) {
				*it = ToUpperASCII(*it);
			}
		}

		template <typename strT>
		inline strT StringToUpperASCII(const strT& s) {
			strT output(s);
			StringToUpperASCII(&output);
			return output;
		}

		// -------------------------------------------------------------------------

		/*===================================================//
			   以下方法主要处理字符串两边的空白部分（Trim）
		//===================================================*/

		enum TrimPositions {
			eTrim_None = 0,
			eTrim_Leading = 1 << 0,
			eTrim_Trailing = 1 << 1,
			eTrim_All = eTrim_Leading | eTrim_Trailing
		};

		template<typename strT>
		TrimPositions TrimStringT(const strT& input, const typename strT::value_type trim_chars[],
			TrimPositions positions, strT* output) {
			const typename strT::size_type last_char = input.length() - 1;
			const typename strT::size_type first_good_char = (positions & eTrim_Leading) ? input.find_first_not_of(trim_chars) : 0;
			const typename strT::size_type last_good_char = (positions & eTrim_Trailing) ? input.find_last_not_of(trim_chars) : last_char;

			if (input.empty() || (first_good_char == strT::npos) || (last_good_char == strT::npos)) {
				bool input_was_empty = input.empty();
				output->clear();
				return input_was_empty ? eTrim_None : positions;
			}

			*output = input.substr(first_good_char, last_good_char - first_good_char + 1);

			return static_cast<TrimPositions>(((first_good_char == 0) ? eTrim_None : eTrim_Leading) |
				((last_good_char == last_char) ? eTrim_None : eTrim_Trailing));
		}

		TrimPositions TrimWhitespace(const std::wstring& input, TrimPositions positions, std::wstring* output);
		TrimPositions TrimWhitespaceASCII(const std::string& input, TrimPositions positions, std::string* output);


		// -------------------------------------------------------------------------

		/*===================================================//
					以下方法主要处理字符串编码转换
	    //===================================================*/

		std::wstring ToStringWide(const char* c, int len, UINT codePage = CP_ACP);
		std::wstring ToStringWide(const std::string& strASCII, UINT codePage = CP_ACP);

		std::string ToStringASCII(const wchar_t* c, int len, UINT codePage = CP_ACP);
		std::string ToStringASCII(const std::wstring& strWide, UINT codePage = CP_ACP);

		// -------------------------------------------------------------------------

		/*=========================================//
				以下方法主要处理数字和字符串转化
		//=========================================*/
		std::string  IntToStringASCII(int num);
		std::wstring IntToStringWide(int num);
		int ASCIIStringToInt(const std::string& str);
		int WideStringToInt(const std::wstring& strW);

		// -------------------------------------------------------------------------

		/*===================================================//
					以下方法主要处理字符串分割
	    //===================================================*/
		std::vector<std::wstring>& SplitWide(wchar_t* source, const wchar_t* token, std::vector<std::wstring>& dsts);
		std::vector<std::string>& SplitASCII(char* source, const char* token, std::vector<std::string>& dsts);

		// -------------------------------------------------------------------------


		/*===================================================//
					以下方法主要处理字符串查找
	    //===================================================*/
		// templated version of my_equal so it could work with both char and wchar_t
		template<typename charT>
		struct str_i_equal {
			str_i_equal( const std::locale& loc ) : loc_(loc) {}
			bool operator()(charT ch1, charT ch2) {
				return std::toupper(ch1, loc_) == std::toupper(ch2, loc_);
			}
		private:
			const std::locale& loc_;
		};

		// find substring (case insensitive)
		template<typename T>
		int ci_find_substr( const T& str1, const T& str2, const std::locale& loc = std::locale() )
		{
			typename T::const_iterator it = std::search( str1.begin(), str1.end(), 
				str2.begin(), str2.end(), str_i_equal<typename T::value_type>(loc) );
			if ( it != str1.end() ) return it - str1.begin();
			else return -1; // not found
		}

		int versioncmp(char const* v1, char const* v2);

	}
}

// -------------------------------------------------------------------------


#endif /* __BASE_STRING_UTIL_H__ */
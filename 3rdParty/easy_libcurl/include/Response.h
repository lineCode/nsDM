/*!
 * \file Response.h
 * \date 2016/02/25 16:14
 *
 * \author lniwn@live.com
 *
 * \brief 
 *
 * \version 1.0
 *
*/

#pragma once
#include "global_define.h"
#include <string>
#include <vector>
#include "ICallbackBase.h"
#include "CaseIMap.h"

namespace easy_libcurl
{
	/*!
	 * \class Response
	 *
	 * \brief 服务器返回的结果
	 *
	 * 参数规则参考python的requests.Response类
	 * \ref http://docs.python-requests.org/en/master/api/#requests.Response
	 *
	 */
	class Response
	{
		friend class Session;
	public:
		std::string encoding; //!< \see content 内容的编码格式, 会自动从HTTP头获取, 用户也可以指定
		std::vector<std::string> cookies;
		caseimap<std::string> headers; //!< 服务器返回的HTTP头
		std::vector<char> content; //!< 服务器返回的原始内容, stream模式请使用\see iter_content
		bool is_redirect; //!< 是否被重定向
		long status_code; //!< HTTP状态码\ref https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
		std::string url; //!< 最后请求的url, 如果没有重定向, 则与用户请求url一致

	public:
		void iter_content(); //!< 遍历\see content, stream模式时使用
		std::wstring text(); //!< 使用\see encoding 编码\see content, 返回编码后的Unicode字符串

	private:
		IterHandler _iter_h;

	public:
		Response(void);
		~Response(void);
	};
}



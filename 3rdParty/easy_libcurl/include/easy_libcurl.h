#pragma once

#include "global_define.h"
#include "Response.h"
#include "Session.h"

namespace easy_libcurl
{
	/*!
	 * \fn bool init()
	 * \brief 初始化函数, 请确保全局调用一次
	 * \param amount 连接池大小
	 * \return true如果初始化成功, 否则返回false
	 */
	bool init();

	/*!
	 * \fn bool uninit()
	 * \brief 反初始化函数, 请确保全局调用一次
	 * \return true如果反初始化成功, 否则返回false
	 */
	bool uninit();

	bool set_max_connects(long amount); //!< 最大并发数
	void prepare_simple_request(RequestParam* kwargs); //!< 简单初始化请求参数
	char* url_encode(const char* data, int length);
	char* url_decode(const char* data, int length);

	bool inner_option(int op, void* param); //!< 直接操作libcurl接口\ref https://curl.haxx.se/libcurl/c/curl_easy_setopt.html
	bool inner_option(EasyHandler handler, int op, void* param);

	Response* request(RequestMethod method, const char* url, RequestParam* kwargs = NULL);
	Response* get(const char* url, RequestParam* kwargs = NULL);
	Response* post(const char* url, const char* data = NULL, long data_size = -1, RequestParam* kwargs = NULL);
	Response* download(const char* url, const wchar_t* save_path = NULL, RequestParam* kwargs = NULL);

	bool get_async(IDelegateInterface* delegate_engine, const char* url, RequestParam* kwargs = NULL);
	bool post_async(IDelegateInterface* delegate_engine, const char* url, const char* data = NULL, long data_size = -1, RequestParam* kwargs = NULL);
	bool download_async(IDelegateInterface* delegate_engine, const char* url, const wchar_t* save_path = NULL, RequestParam* kwargs = NULL);
	bool request_async(IDelegateInterface* delegate_engine, RequestMethod method, const char* url, RequestParam* kwargs = NULL);
}


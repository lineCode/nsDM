/*!
 * \file global_define.h
 *
 * \author lniwn@live.com
 *
 * \date 二月 2016
 *
 * \version 1.0
 * 
 */

#pragma once

#include <tchar.h>
#include <string>
#include <list>
#include <tuple>
#include <vector>
#include "CaseIMap.h"

namespace easy_libcurl
{

	/*!
	 * \enum RequestMethod
	 *
	 * \brief HTTP(s)请求的方法
	 *
	 */
	enum RequestMethod
	{
		METHOD_GET,
		METHOD_POST,
		METHOD_HEAD, //!< 未实现
		METHOD_PUT, //!< 未实现
		METHOD_PATCH, //!< 未实现
		METHOD_DELETE, //!< 未实现
		METHOD_OPTIONS //!< 未实现
	};

	/*!
	 * \brief 用于\see RequestParam 的掩码
	 *
	 */
	namespace RP_Mask
	{
		const unsigned long rp_none                 = 0;

		const unsigned long rp_method               = 1<<0x0;
		const unsigned long rp_url                  = 1<<0x1;
		const unsigned long rp_params               = 1<<0x2;
		const unsigned long rp_data_field           = 1<<0x3; //!< post data: simple fileds
		const unsigned long rp_data_multi           = 1<<0x4; //!< post data: multipart/formdata, must set "Content-Type"
		const unsigned long rp_headers              = 1<<0x5;
		const unsigned long rp_cookies              = 1<<0x6;
		const unsigned long rp_auth                 = 1<<0x7;
		const unsigned long rp_request_time         = 1<<0x8;
		const unsigned long rp_connect_time         = 1<<0x9;
		const unsigned long rp_allow_redirects      = 1<<0xa;
		const unsigned long rp_proxy                = 1<<0xb;
		const unsigned long rp_cert                 = 1<<0xc;
		const unsigned long rp_stream               = 1<<0xd;
		const unsigned long rp_max_redirects        = 1<<0xe;
		const unsigned long rp_url_encode           = 1<<0xf;
		
		const unsigned long rp_all                  = static_cast<unsigned long>(-1);
	}

	/*!
	 * \struct RequestParam
	 *
	 * \brief 请求参数结构体
	 *
	 * 参数规则参考python的requests模块
	 * \ref http://docs.python-requests.org/en/master/api/
	 *
	 * \param method 请求的方法\see RequestMethod
	 * \param url 请求的url地址
	 * \param params 请求参数列表, eg: params.push_back("param=value")
	 * \param data HTTP(s) body部分
	 * \param headers HTTP(s) head部分, eg: headers.push_back("Accept: *")
	 * \param cookies 自定义的cookie, eg: "author=lniwn; tool=curl"
	 * \param auth 用户名和密码, eg: "account":"password")
	 * \param request_timeout 请求超时时间, 单位: 秒, 默认值 0(永不超时)
	 * \param connect_timeout 连接超时时间, 单位：秒, 默认值 300
	 * \param allow_redirects 如果允许POST/PUT/DELETE方法重定向，则设置为true, 否则设置为false
	 * \param proxy 代理地址
	 * \param cert 为空时不进行SSL证书校验; 仅有一个参数时作为cert文件路径; 两个参数时作为("cert path", "key path")
	 * \param bitmask 指定结构体中有效字段
	 * \param stream true:不会一次性读取服务器返回值到内存；false:一次性读取服务器返回值到内存
	 * \param max_redirects 允许最大重定向次数, 默认-1，表示不限制
	 * \param url_encode 是否对body字段进行url编码
	 */
	struct RequestParam 
	{
		RequestMethod method;
		std::string url;
		std::list<std::string> params;
		std::vector<char> data;
		caseimap<std::string> headers;
		std::list<std::string> cookies;
		std::string auth;
		long request_timeout;
		long connect_timeout;
		std::string proxy;
		std::list<std::string> cert;
		unsigned long bitmask;
		long max_redirects;
		bool allow_redirects;
		bool stream;
		bool url_encode;

		RequestParam()
		{
			bitmask = 0;
		}
	};

	namespace UserAgent
	{
		_declspec(selectany) const char* IE10 = 
			"Mozilla/5.0 (compatible; WOW64; MSIE 10.0; Windows NT 6.2)";
		_declspec(selectany) const char* IE11 = 
			"Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko";
		_declspec(selectany) const char* Firefox = 
			"Mozilla/5.0 (Windows NT 6.2; WOW64; rv:21.0) Gecko/20100101 Firefox/21.0";
		_declspec(selectany) const char* Chrome =
			"Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/48.0.2564.97 Safari/537.36";
	}

	/*!
	 * \fn int download_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
	 * \brief 下载/上传进度回调函数原型
	 *
	 * \ref https://curl.haxx.se/libcurl/c/CURLOPT_XFERINFOFUNCTION.html
	 */
	typedef int (*progress_callbackT)(void *clientp, long long dltotal, long long dlnow, long long ultotal, long long ulnow);

	typedef void* EasyHandler;
	typedef volatile long* IterHandler;
	typedef void* HeaderList;

	/*!
	 * \fn size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
	 * \brief 下载回调函数原型
	 *
	 * 自定义处理服务器返回数据
	 *
	 * \ref https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html
	 */
	typedef size_t (*write_callbackT)(char *ptr, size_t size, size_t nmemb, void *userdata);

	/*!
	 * \fn size_t read_callback(char *buffer, size_t size, size_t nitems, void *instream)
	 * \brief 上传回调函数原型
	 *
	 * 自定义上传数据
	 * \ref https://curl.haxx.se/libcurl/c/CURLOPT_READFUNCTION.html
	 */
	typedef size_t (*read_callbackT)(char *buffer, size_t size, size_t nitems, void *instream);

	/*!
	 * \fn void content_callback(char* data, int chunk_size, bool decode_unicode)
	 * \brief 服务器返回内容获取的回调函数原型
	 *
	 * \param data 服务器返回数据
	 * \param chunk_size 数据大小
	 * \param decode_unicode 是否以\see Response encoding解码为Unicode
	 */
	typedef void (*content_callbackT)(char* data, size_t chunk_size, bool decode_unicode);

#if defined(DEBUG) || defined(_DEBUG)

#ifndef TODO
#define _STR(x) #x
#define STR(x) _STR(x)
#define TODO(x) __pragma(message("TODO: "_STR(x) " :: " __FILE__ "@"STR(__LINE__)))
#endif

#else

#ifndef TODO
#define TODO(x) (void*)0
#endif
	
#endif
}
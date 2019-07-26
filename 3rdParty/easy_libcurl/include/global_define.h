/*!
 * \file global_define.h
 *
 * \author lniwn@live.com
 *
 * \date ���� 2016
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
	 * \brief HTTP(s)����ķ���
	 *
	 */
	enum RequestMethod
	{
		METHOD_GET,
		METHOD_POST,
		METHOD_HEAD, //!< δʵ��
		METHOD_PUT, //!< δʵ��
		METHOD_PATCH, //!< δʵ��
		METHOD_DELETE, //!< δʵ��
		METHOD_OPTIONS //!< δʵ��
	};

	/*!
	 * \brief ����\see RequestParam ������
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
	 * \brief ��������ṹ��
	 *
	 * ��������ο�python��requestsģ��
	 * \ref http://docs.python-requests.org/en/master/api/
	 *
	 * \param method ����ķ���\see RequestMethod
	 * \param url �����url��ַ
	 * \param params ��������б�, eg: params.push_back("param=value")
	 * \param data HTTP(s) body����
	 * \param headers HTTP(s) head����, eg: headers.push_back("Accept: *")
	 * \param cookies �Զ����cookie, eg: "author=lniwn; tool=curl"
	 * \param auth �û���������, eg: "account":"password")
	 * \param request_timeout ����ʱʱ��, ��λ: ��, Ĭ��ֵ 0(������ʱ)
	 * \param connect_timeout ���ӳ�ʱʱ��, ��λ����, Ĭ��ֵ 300
	 * \param allow_redirects �������POST/PUT/DELETE�����ض���������Ϊtrue, ��������Ϊfalse
	 * \param proxy �����ַ
	 * \param cert Ϊ��ʱ������SSL֤��У��; ����һ������ʱ��Ϊcert�ļ�·��; ��������ʱ��Ϊ("cert path", "key path")
	 * \param bitmask ָ���ṹ������Ч�ֶ�
	 * \param stream true:����һ���Զ�ȡ����������ֵ���ڴ棻false:һ���Զ�ȡ����������ֵ���ڴ�
	 * \param max_redirects ��������ض������, Ĭ��-1����ʾ������
	 * \param url_encode �Ƿ��body�ֶν���url����
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
	 * \brief ����/�ϴ����Ȼص�����ԭ��
	 *
	 * \ref https://curl.haxx.se/libcurl/c/CURLOPT_XFERINFOFUNCTION.html
	 */
	typedef int (*progress_callbackT)(void *clientp, long long dltotal, long long dlnow, long long ultotal, long long ulnow);

	typedef void* EasyHandler;
	typedef volatile long* IterHandler;
	typedef void* HeaderList;

	/*!
	 * \fn size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
	 * \brief ���ػص�����ԭ��
	 *
	 * �Զ��崦���������������
	 *
	 * \ref https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html
	 */
	typedef size_t (*write_callbackT)(char *ptr, size_t size, size_t nmemb, void *userdata);

	/*!
	 * \fn size_t read_callback(char *buffer, size_t size, size_t nitems, void *instream)
	 * \brief �ϴ��ص�����ԭ��
	 *
	 * �Զ����ϴ�����
	 * \ref https://curl.haxx.se/libcurl/c/CURLOPT_READFUNCTION.html
	 */
	typedef size_t (*read_callbackT)(char *buffer, size_t size, size_t nitems, void *instream);

	/*!
	 * \fn void content_callback(char* data, int chunk_size, bool decode_unicode)
	 * \brief �������������ݻ�ȡ�Ļص�����ԭ��
	 *
	 * \param data ��������������
	 * \param chunk_size ���ݴ�С
	 * \param decode_unicode �Ƿ���\see Response encoding����ΪUnicode
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
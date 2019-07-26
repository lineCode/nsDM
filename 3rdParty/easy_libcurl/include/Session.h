/*!
 * \file Session.h
 * \date 2016/02/25 16:16
 *
 * \author lniwn@live.com
 *
 * \brief 
 *
 *	MSVC++ 14.0 _MSC_VER == 1900 (Visual Studio 2015)
 *	MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)
 *	MSVC++ 11.0 _MSC_VER == 1700 (Visual Studio 2012)
 *	MSVC++ 10.0 _MSC_VER == 1600 (Visual Studio 2010)
 *	MSVC++ 9.0  _MSC_VER == 1500 (Visual Studio 2008)
 *	MSVC++ 8.0  _MSC_VER == 1400 (Visual Studio 2005)
 *	MSVC++ 7.1  _MSC_VER == 1310 (Visual Studio 2003)
 *	MSVC++ 7.0  _MSC_VER == 1300
 *	MSVC++ 6.0  _MSC_VER == 1200
 *	MSVC++ 5.0  _MSC_VER == 1100
 *
 * \version 1.0
 *
 */

#pragma once
#include "global_define.h"
#include "Response.h"
#include <string>
#include <tuple>
#include "ICallbackBase.h"
#include "Win32DelegateEngine.h"


namespace easy_libcurl
{
	/*!
	 * \class Session
	 *
	 * \brief HTTP(s)�Ự������
	 *
	 * ��������ο�python��requests.Session��
	 * \ref http://docs.python-requests.org/en/master/api/#requests.Session
	 *
	 */
	class Session
	{
	public:
		RequestParam request_param;
		
	public:
		static Session* create_session(ICallbackBase* _callback = NULL);
		static void destroy_session(Session* ss);
		//Session* clone() const;

		/*!
		 * \fn void reset()
		 * \brief ��session����Ϊ��ʼ״̬
		 *
		 * \ref https://curl.haxx.se/libcurl/c/curl_easy_reset.html
		 *
		 * \note ���ı䣺�Ѿ��򿪵����ӡ�session���桢dns���桢cookies���ڲ�����״̬
		 */
		void reset();

		Response* get(const char* url, RequestParam* kwargs = NULL);
		Response* post(const char* url, const char* data = NULL, long data_size = -1, RequestParam* kwargs = NULL);
		Response* request(RequestMethod method, const char* url, RequestParam* kwargs = NULL);

		/*!
		 * \fn Response* download(const char* url, const wchar_t* save_path = NULL, RequestParam* kwargs = NULL)
		 * \brief �����ļ�
		 * \param url ����url
		 * \param save_path ��������·�������ĩβΪ\����/�����url����ȡ�ļ���
		 * \param kwargs �������
		 *
		 */
		Response* download(const char* url, const wchar_t* save_path = NULL, RequestParam* kwargs = NULL);
		//Response send(const RequestParam* prepared);
		
		void prepare_simple_request(RequestParam* kwargs);

		const char* errorstr();
		void init_response(Response* resp);
		void cancel_operation();
		std::string url_encode(const char* data, int length);
		std::string url_decode(const char* data, int length);

		/*!
		 * \name pause
		 * \brief pause and unpause a connection
		 *
		 * ��ͣ/�ָ��ϴ������ز��������ܿ��̵߳��ã��Ƽ���\see set_progress_callback���Ȼص������е���
		 * ���ַ�ʽ��ͣ/�ָ�session:�ٵ��ø�pauseϵ�к��������ڽ��Ȼص������з���(CURL_READFUNC_PAUSE and CURL_WRITEFUNC_PAUSE)
		 *
		 * \param pause true��ʾ��ͣ��false��ʾ�ָ�
		 *
		 * \{
		 */
		bool pause_read(bool pause);
		bool pause_write(bool pause);
		bool pause_all(bool pause);
		/*! \} */

		/*!
		 * \name async call
		 */
		bool get_async(IDelegateInterface* delegate_engine, const char* url, RequestParam* kwargs = NULL);
		bool post_async(IDelegateInterface* delegate_engine, const char* url, const char* data = NULL, long data_size = -1, RequestParam* kwargs = NULL);
		bool download_async(IDelegateInterface* delegate_engine, const char* url, const wchar_t* save_path = NULL, RequestParam* kwargs = NULL);
		bool request_async(IDelegateInterface* delegate_engine, RequestMethod method, const char* url, RequestParam* kwargs = NULL);
		/*! \}*/

	private:
		static Session* create_session(EasyHandler h, ICallbackBase* _callback);

		bool prepare_request(RequestParam* kwargs);

		bool prepare_method(RequestParam* kwargs);
		bool prepare_headers(RequestParam* kwargs);
		bool prepare_timeout(RequestParam* kwargs);
		std::string prepare_params(RequestParam* kwargs);
		bool prepare_body(RequestParam* kwargs);
		bool prepare_cookies(RequestParam* kwargs);
		bool prepare_auth(RequestParam* kwargs);
		bool prepare_redirects(RequestParam* kwargs);
		bool prepare_proxy(RequestParam* kwargs);
		bool prepare_cert(RequestParam* kwargs);
		bool prepare_url(const char* url, const char* param);
		void prepare_stream(RequestParam* kwargs);

		void handle_response();
		static size_t header_callback(char* buffer, size_t size, size_t nitems, void* userdata);
		static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
		static int progress_callback(void *clientp, long long dltotal, 
			long long dlnow, long long ultotal, long long ulnow);

		bool set_progress_callback(progress_callbackT cb, void* param);
		bool set_write_callback(write_callbackT wc, void* param);
		//bool set_read_callback(read_callbackT rc, void* param);
		bool set_header_callback(write_callbackT wc, void* param);

		static VOID CALLBACK request_apc(_In_ ULONG_PTR dwParam);

		void reinit();

	private:
		Session(void);
		~Session(void);

		EasyHandler    _handler;
		int            _eCode;
		Response       _response;
		ICallbackBase* _callback_handle;
		LONG           _iter_h;
		LONG           _cancelled;
		bool           _is_stream;
		bool           _immediate_mode;
		HeaderList     _headers;
		std::wstring   _download_path;
		FILE*          _download_file;
	};
}


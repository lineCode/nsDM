#pragma once

#include "global_define.h"
#include "Response.h"
#include "Session.h"

namespace easy_libcurl
{
	/*!
	 * \fn bool init()
	 * \brief ��ʼ������, ��ȷ��ȫ�ֵ���һ��
	 * \param amount ���ӳش�С
	 * \return true�����ʼ���ɹ�, ���򷵻�false
	 */
	bool init();

	/*!
	 * \fn bool uninit()
	 * \brief ����ʼ������, ��ȷ��ȫ�ֵ���һ��
	 * \return true�������ʼ���ɹ�, ���򷵻�false
	 */
	bool uninit();

	bool set_max_connects(long amount); //!< ��󲢷���
	void prepare_simple_request(RequestParam* kwargs); //!< �򵥳�ʼ���������
	char* url_encode(const char* data, int length);
	char* url_decode(const char* data, int length);

	bool inner_option(int op, void* param); //!< ֱ�Ӳ���libcurl�ӿ�\ref https://curl.haxx.se/libcurl/c/curl_easy_setopt.html
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


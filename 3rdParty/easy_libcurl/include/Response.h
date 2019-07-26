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
	 * \brief ���������صĽ��
	 *
	 * ��������ο�python��requests.Response��
	 * \ref http://docs.python-requests.org/en/master/api/#requests.Response
	 *
	 */
	class Response
	{
		friend class Session;
	public:
		std::string encoding; //!< \see content ���ݵı����ʽ, ���Զ���HTTPͷ��ȡ, �û�Ҳ����ָ��
		std::vector<std::string> cookies;
		caseimap<std::string> headers; //!< ���������ص�HTTPͷ
		std::vector<char> content; //!< ���������ص�ԭʼ����, streamģʽ��ʹ��\see iter_content
		bool is_redirect; //!< �Ƿ��ض���
		long status_code; //!< HTTP״̬��\ref https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
		std::string url; //!< ��������url, ���û���ض���, �����û�����urlһ��

	public:
		void iter_content(); //!< ����\see content, streamģʽʱʹ��
		std::wstring text(); //!< ʹ��\see encoding ����\see content, ���ر�����Unicode�ַ���

	private:
		IterHandler _iter_h;

	public:
		Response(void);
		~Response(void);
	};
}



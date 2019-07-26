#pragma once


namespace easy_libcurl
{
	class ICallbackBase
	{
	public:

		/*!
		 * \fn void on_read(char *buffer, size_t chunk_size)
		 *
		 * \brief �ӱ���Ӳ�̶�ȡ�ļ��Ļص�
		 * 
		 * \note ��δʵ��
		 */
		virtual void on_read(char *buffer, size_t chunk_size) { buffer; chunk_size;}

		/*!
		 * \fn void on_write(char *ptr, size_t chunk_size)
		 *
		 * \brief ��ȡ�������������ݵĻص�
		 * 
		 * \note ������HTTPͷ
		 */
		virtual void on_write(char *ptr, size_t chunk_size) { ptr; chunk_size;}

		/*!
		 * \fn void on_progress(long long dltotal, long long dlnow, long long ultotal, long long ulnow)
		 *
		 * \brief ����/�ϴ����Ȼص�
		 * 
		 * \param dltotal ��������(�ֽ�)
		 * \param dlnow ��ǰ��������(�ֽ�)
		 * \param ultotal ���ϴ���(�ֽ�)
		 * \param ulnow ��ǰ���ϴ���(�ֽ�)
		 */
		virtual void on_progress(long long dltotal, 
			long long dlnow, long long ultotal, long long ulnow){ dltotal;dlnow;ultotal;ulnow;}

		/*!
		 * \fn void on_done(const void* usrp)
		 *
		 * \brief �첽����������ɵĻص�����
		 */
		virtual void on_done(const void* usrp){usrp;}

		virtual ~ICallbackBase(void) {}
	};
}


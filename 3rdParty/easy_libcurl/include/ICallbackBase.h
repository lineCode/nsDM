#pragma once


namespace easy_libcurl
{
	class ICallbackBase
	{
	public:

		/*!
		 * \fn void on_read(char *buffer, size_t chunk_size)
		 *
		 * \brief 从本地硬盘读取文件的回调
		 * 
		 * \note 暂未实现
		 */
		virtual void on_read(char *buffer, size_t chunk_size) { buffer; chunk_size;}

		/*!
		 * \fn void on_write(char *ptr, size_t chunk_size)
		 *
		 * \brief 获取服务器返回数据的回调
		 * 
		 * \note 不包括HTTP头
		 */
		virtual void on_write(char *ptr, size_t chunk_size) { ptr; chunk_size;}

		/*!
		 * \fn void on_progress(long long dltotal, long long dlnow, long long ultotal, long long ulnow)
		 *
		 * \brief 下载/上传进度回调
		 * 
		 * \param dltotal 总下载量(字节)
		 * \param dlnow 当前已下载量(字节)
		 * \param ultotal 总上传量(字节)
		 * \param ulnow 当前已上传量(字节)
		 */
		virtual void on_progress(long long dltotal, 
			long long dlnow, long long ultotal, long long ulnow){ dltotal;dlnow;ultotal;ulnow;}

		/*!
		 * \fn void on_done(const void* usrp)
		 *
		 * \brief 异步网络请求完成的回调函数
		 */
		virtual void on_done(const void* usrp){usrp;}

		virtual ~ICallbackBase(void) {}
	};
}


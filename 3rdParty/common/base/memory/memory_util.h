#pragma once
namespace base
{
	namespace memory
	{
		template <class T>
		void delete_array(T* ptr)
		{
			delete[] ptr;
		}
	}
}
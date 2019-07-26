/* -------------------------------------------------------------------------
//	FileName	��	objref.h
//	Creator		��	yanbozhao
//	CreateTime	��	2015/05/06 ������ 10:50:11
//	Description	��
//
// -----------------------------------------------------------------------*/
#ifndef __BASE_OBJREF_H__
#define __BASE_OBJREF_H__

// -------------------------------------------------------------------------

namespace base {
	template <typename ObjT>
	class ObjRefT
	{
	public:
		ObjRefT(ObjT* pT)
			: ref_(1)
			, pT_(pT) {
		}

		virtual ~ObjRefT() {
		}

	public:
		virtual ULONG AddRef() {
			return InterlockedIncrement(&ref_);
		}

		virtual void Release() {
			if (0 == InterlockedDecrement(&ref_)) {
				delete pT_;
			}
		}

	private:
		ULONG	ref_;
		ObjT*	pT_;
	};
}

// -------------------------------------------------------------------------
#endif	// __BASE_OBJREF_H__

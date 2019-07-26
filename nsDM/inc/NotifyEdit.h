#pragma once
#include "DUIEdit.h"

using DM::DMCode;


class CNotifyEdit: public DM::DUIEdit
{
	DMDECLARE_CLASS_NAME(CNotifyEdit, L"notifyEdit", DM::DMREG_Window)
public:
	CNotifyEdit(void);
	~CNotifyEdit(void);

protected:
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	DM_BEGIN_MSG_MAP()
		MSG_WM_KEYDOWN(OnKeyDown)
	DM_END_MSG_MAP()
};


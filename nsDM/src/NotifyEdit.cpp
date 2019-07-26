#include "StdAfx.h"
#include "NotifyEdit.h"

using DM::DM_ECODE_FAIL;
using DM::DM_ECODE_OK;



CNotifyEdit::CNotifyEdit(void)
{
}


CNotifyEdit::~CNotifyEdit(void)
{
}

void CNotifyEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	DM_SendMessage(EM_SETEVENTMASK, 0, ENM_CHANGE);
	SetMsgHandled(FALSE);
}

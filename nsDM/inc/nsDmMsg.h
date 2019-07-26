#pragma once
#include <WinUser.h>

namespace NSDM
{
	enum NSDM_MSG
	{
		UM_NSDM_MIN = WM_USER + 0x88,
		UM_NSDM_CLOSE,
		UM_NSDM_CANCEL,
		UM_NSDM_QUERY_CANCEL,
		UM_NSDM_OPEN_URL,
		UM_NSDM_EXIT
	};
}

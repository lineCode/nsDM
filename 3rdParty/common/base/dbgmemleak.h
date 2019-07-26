/* -------------------------------------------------------------------------
//	FileName	��	dbgmemleak.h
//	Creator		��	yanbozhao
//	CreateTime	��	2015/08/21 ������ 11:06:11
//	Description	��
//
// -----------------------------------------------------------------------*/
#ifndef __BASE_DBGMEMLEAK_H__
#define __BASE_DBGMEMLEAK_H__

#define	_CRTDBG_MAP_ALLOC	// ʹ��CrtMemoryDebug

#include <crtdbg.h>
// -------------------------------------------------------------------------

/*===============================================================//
	����Ҫ����ڴ�й¶���򽫷����ڴ�ĺ�����������滻�����¶���ĺ꣬
	����ֻ��DEBUGģʽ��ʹ�á���
	#ifdef _DEBUG
	#define new DEBUG_NEW
	#endif
//===============================================================*/

inline int _CrtNormalBlockType() {
	static int tmpFlag =
		_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	return _NORMAL_BLOCK;
}

#define DEBUG_MALLOC(s)		   _malloc_dbg(s, _CrtNormalBlockType(), __FILE__, __LINE__)
#define DEBUG_CALLOCc(c, s)    _calloc_dbg(c, s, _CrtNormalBlockType(), __FILE__, __LINE__)
#define DEBUG_REALLOC(p, s)	   _realloc_dbg(p, s, _CrtNormalBlockType(), __FILE__, __LINE__)
#define DEBUG_NEW				new(_CrtNormalBlockType(), __FILE__, __LINE__)

// -------------------------------------------------------------------------
#endif	// __BASE_DBGMEMLEAK_H__
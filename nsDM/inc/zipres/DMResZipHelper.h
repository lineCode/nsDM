//-------------------------------------------------------
// Copyright (c) DuiMagic
// All rights reserved.
// 
// File Name: DMResHelper.h 
// File Des:  ����Res�����ļ�
// File Summary: 
// Cur Version: 1.0
// Author:
// Create Data:
// History:
// 		<Author>	<Time>		<Version>	  <Des>
//      guoyou		2015-4-28	1.0			
//-------------------------------------------------------
#pragma once
namespace DM
{

#define MAX_RES_TYPE		   30						 // ��Դ����Ԥ���壬��󳤶Ȳ�����30
#define MAX_RES_NAME		   100						 // ע�⣺����Դ����ʱ����󳤶Ȳ�Ҫ����MAX_RES_NAME
#define DMRES_DMINDEX		   _T("dmindex.xml")		 // �ļ�����Դ���ļ�ӳ����������ļ���
#define DMRES_THEMES_FOLDER    _T("\\themes\\")
#define DMRES_LAYOUT_FOLDER    _T("\\layout\\")

	/// <summary>
	///		����Res���������
	/// </summary>
	class DMZipItem
	{
	public:
		DMZipItem(LPCWSTR lpszType=L"", LPCWSTR lpszName=L"", LPCWSTR lpszPath = L"");

	public:
		wchar_t m_szType[MAX_RES_TYPE+1];
		wchar_t m_szName[MAX_RES_NAME+1];
		wchar_t m_szPath[MAX_PATH];
	};
	typedef DMZipItem* DMZipItemPtr;

	/// <summary>
	///		����ResZipʹ�õ�Array
	/// </summary>
	class DMZipItemArray:public DMRefNum
						 ,public DMArrayT<DMZipItemPtr>
	{
	public:
		DMZipItemArray(){}
		virtual ~DMZipItemArray();
	public:
		virtual void PreArrayObjRemove(const DMZipItemPtr &obj);
		virtual bool EqualArrayObj(const DMZipItemPtr &objsrc, const DMZipItemPtr &objdest);
	public:
		CStringW        m_strThemeName;
	};
	typedef DMZipItemArray* DMZipItemArrayPtr;

}//namespace DM
#pragma once
#include "DMResZipHelper.h"
#include "DMZipHelper.h"
namespace DM
{
#define  RES_INDEXFILE_CHECK          1// �����ļ��Ƿ����ǿ��У�����
	/// <summary>
	///     ����zipʾ�������ô���ļ���һ��
	///		����Res����ļ���Ϊ���½ṹ:�������ò����������󣬿����Լ��޸�һ�ݣ��Լ�ע���ȥ
	///     Res�ļ��а����������ļ��У�1.layout�ļ��� 2.themes�ļ���
	///		1.1.layout�ļ��ж��������в���xml��skin��style��ȫ��xml
	///     2.1.themes�ļ����¶����˶�������
	///     3.ÿ���ļ����¾���һ��dmindex.xml����ʶ��Դ����
	/// </summary>
	class DMResZipImpl:public IDMRes,public DMArrayT<DMZipItemArrayPtr>
	{
		DMDECLARE_CLASS_NAME(DMResZipImpl,L"DMResZipImpl",DMREG_Res);
	public:
		DMResZipImpl();
		~DMResZipImpl();
	public:
		virtual DMCode LoadResPack(WPARAM wp, LPARAM lp);
		virtual DMCode IsItemExists(LPCWSTR lpszType, LPCWSTR lpszName,LPCWSTR lpszThemeName=NULL);
		virtual DMCode GetItemSize(LPCWSTR lpszType, LPCWSTR lpszName, unsigned long& ulSize,LPCWSTR lpszThemeName=NULL);
		virtual DMCode GetItemBuf(LPCWSTR lpszType, LPCWSTR lpszName, LPVOID lpBuf, unsigned long ulSize,LPCWSTR lpszThemeName=NULL);
		virtual DMCode LoadTheme(WPARAM wp, LPARAM lp);// ��ʹ�����ַ�ʽ
		virtual DMCode SetCurTheme(LPCWSTR lpszName, LPCWSTR lpszOldName=NULL);
		virtual DMCode SendExpandInfo(WPARAM wp, LPARAM lp);  ///< lpΪ0ʱ���ѳ��ȸ�ֵ��*wp

	public:// ����
		DMCode ParseThemes(LPCWSTR lpszIndexPath);
		DMCode ParseIndex(LPCWSTR lpszIndexPath,DMZipItemArray** ppItem);
		LPCWSTR GetItemPath(LPCWSTR lpszType,LPCWSTR lpszName,LPCWSTR lpszThemeName);
		DMZipItemArrayPtr FindResItemObj(LPCWSTR lpszName);

	public:
		DM_BEGIN_ATTRIBUTES()
			DM_bool_ATTRIBUTE(L"bloop", m_bThemeLoop, DM_ECODE_OK)
			DM_STRING_ATTRIBUTE(L"name", m_strCurTheme, DM_ECODE_OK)
			DM_bool_ATTRIBUTE(L"boutstyle", m_bOutStyle, DM_ECODE_OK)
		DM_END_ATTRIBUTES()

	protected:
		virtual void PreArrayObjRemove(const DMZipItemArrayPtr &obj);

	protected:
		DM::CStringW                          m_strDir;
		DM::CStringW                          m_strCurTheme;       // ��ǰʹ�õ�theme Key
		DMZipItemArrayPtr					  m_pCurTheme;         // ��ǰʹ�õ�theme
		bool                                  m_bThemeLoop;        // �Ƿ�����ѭ������theme(��ǰtheme���Ҳ�����Դ,ֱ��ȥ����theme�в���)
		bool                                  m_bOutStyle;         // ��ǰ���ⲿģʽ���ⲿǿ�Ƹı���skin�ص����ݣ���ʱ���е�themeӦ�ö���Ҫˢ��)
		DMSmartPtrT<DMZipItemArray>			  m_pLayout;		   // Layout���б�
		CDMZipHelper						  m_zipFile;           // zip����
	};


}//namespace DM
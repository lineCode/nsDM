#include "stdafx.h"
#include "DMResZipImpl.h"
#include "DMResZipParam.h"

namespace DM
{
	DMResZipImpl::DMResZipImpl()
	{
		m_bThemeLoop = true;
		m_pCurTheme  = NULL;
		m_bOutStyle  = false;
	}

	DMResZipImpl::~DMResZipImpl()
	{
		RemoveAll();
	}

	DMCode DMResZipImpl::LoadResPack(WPARAM wp, LPARAM lp)
	{
		DMCode iErr = DM_ECODE_OK;
		do 
		{
			if (0 == wp||IsBadReadPtr((void*)wp,4))
			{
				break;
			}
			DMResZipParam *pZipParam = (DMResZipParam*)wp;
			if (pZipParam->m_bPERes)// ��Ƕ��exe�е�
			{
				HINSTANCE hInst = (NULL == pZipParam->m_PEResInfo.hInst)?GetModuleHandle(NULL):pZipParam->m_PEResInfo.hInst;
				if (!m_zipFile.LoadFromPE(hInst, pZipParam->m_PEResInfo.lpszResName,pZipParam->m_PEResInfo.lpszResType))
				{
					break;
				}
			}
			else// �ⲿzip
			{
				wchar_t szPath[MAX_PATH] = {0};
				if (!GetRootFullPath((LPWSTR)pZipParam->m_lpszZipPath,szPath,MAX_PATH))
				{
					break;
				}
				if (!m_zipFile.LoadFromFile(szPath)) 
				{
					break;
				}
			}
			m_zipFile.SetPassword(pZipParam->m_lpszPsw);

			m_pLayout.Release();// ���ͷ�ԭʼ�������-----------��
			m_pCurTheme = NULL; // ��ǰʹ�õ�������ÿ�---------��
			RemoveAll();		// ���ͷ�ԭʼthemes-------------��

			// ����Layout
			m_pLayout.Attach(new DMZipItemArray);
			CStringW strLayoutPathIndex = DMRES_LAYOUT_FOLDER;strLayoutPathIndex += DMRES_DMINDEX;
			iErr = ParseIndex(strLayoutPathIndex, &m_pLayout);
			

			// ����themes
			CStringW strThemesPathIndex = DMRES_THEMES_FOLDER;strThemesPathIndex += DMRES_DMINDEX;
			iErr = ParseThemes(strThemesPathIndex);
		} while (false);
		return iErr;
	}

	DMCode DMResZipImpl::IsItemExists(LPCWSTR lpszType, LPCWSTR lpszName,LPCWSTR lpszThemeName)
	{
		DMCode iErr = DM_ECODE_FAIL;
		do 
		{
			if (NULL == lpszType||NULL == lpszName)
			{
				break;
			}
			DMZipItem Item(lpszType, lpszName);// ����������EqualArrayObj��ֻҪǰ����������ȣ�����Ϊ�Ѵ���
			if (0 == _wcsicmp(lpszType, RES_LAYOUT))
			{
				if (DM_INVALID_VALUE == m_pLayout->FindObj(&Item))
				{
					break;
				}
			}
			else
			{
				DMZipItemArrayPtr pTheme = FindResItemObj(lpszThemeName);
				if (pTheme && DM_INVALID_VALUE == pTheme->FindObj(&Item))
				{
					if (false == m_bThemeLoop)
					{
						break;
					}
					bool bFind = false;// ������themes�в���
					int nCount = (int)GetCount();
					for (int i=0;i<nCount;i++)
					{
						DMZipItemArrayPtr &pCur = GetObj(i);
						if (DM_INVALID_VALUE !=pCur->FindObj(&Item))
						{
							bFind = true;
							break;
						}
					}
					if (false == bFind)
					{
						break;
					}
				}
			}
			iErr = DM_ECODE_OK;
		} while (false);
		return iErr;
	}

	DMCode DMResZipImpl::GetItemSize(LPCWSTR lpszType, LPCWSTR lpszName, unsigned long& ulSize,LPCWSTR lpszThemeName)
	{
		DMCode iErr = DM_ECODE_FAIL;
		size_t size = 0;
		do 
		{
			LPCWSTR lpszFilePath = GetItemPath(lpszType, lpszName,lpszThemeName);
			if (NULL == lpszFilePath) 
			{
				break;
			}
			if (L'\\' == lpszFilePath[0]||L'/'==lpszFilePath[0])// ��ǰ�治�ܴ�\\����ȻZIP�޷�ʶ��
			{
				lpszFilePath = lpszFilePath+1;
			}

			ZIP_FIND_DATA zfd;
			HANDLE hf = m_zipFile.FindFirstFile(lpszFilePath,&zfd);
			if (INVALID_HANDLE_VALUE == hf) 
			{
				break;
			}
			m_zipFile.FindClose(hf);
			ulSize = zfd.nFileSizeUncompressed;
			iErr = DM_ECODE_OK;
		} while (false);
		if (!DMSUCCEEDED(iErr))
		{
			CStringW strInfo;
			strInfo.Format(L"Res��Դ��%s:%s��ȡsizeʧ��",lpszType,lpszName);
			DMASSERT_EXPR(0,strInfo);
		}
		return iErr;
	}

	DMCode DMResZipImpl::GetItemBuf(LPCWSTR lpszType, LPCWSTR lpszName, LPVOID lpBuf, unsigned long ulSize,LPCWSTR lpszThemeName)
	{
		DMCode iErr = DM_ECODE_FAIL;
		do 
		{
			LPCWSTR lpszFilePath = GetItemPath(lpszType, lpszName,lpszThemeName);
			if (NULL == lpszFilePath) 
			{
				break;
			}
			if (L'\\' == lpszFilePath[0]||L'/'==lpszFilePath[0])// ��ǰ�治�ܴ�\\����ȻZIP�޷�ʶ��
			{
				lpszFilePath = lpszFilePath+1;
			}

			CDMZipFile zf;
			if (!m_zipFile.GetFile(lpszFilePath, zf))
			{
				break;
			}
			if (ulSize<zf.GetSize())
			{
				break;
			}
			memcpy(lpBuf,zf.GetData(),zf.GetSize());
			iErr = DM_ECODE_OK;
		} while (false);
		if (!DMSUCCEEDED(iErr))
		{
			CStringW strInfo;
			strInfo.Format(L"Res��Դ��%s:%s��ȡbufʧ��",lpszType,lpszName);
			DMASSERT_EXPR(0,strInfo);
		}
		return iErr;
	}

	DMCode DMResZipImpl::LoadTheme(WPARAM wp, LPARAM lp)
	{
		return DM_ECODE_NOTIMPL;
	}

	DMCode DMResZipImpl::SetCurTheme(LPCWSTR lpszName, LPCWSTR lpszOldName)
	{
		DMCode iErr = DM_ECODE_FAIL;
		do 
		{	
			if (lpszOldName)
			{// �����ɵ�theme����,���۳ɹ���񣬶���ȡ��
				memcpy((void*)lpszOldName,m_strCurTheme,m_strCurTheme.GetLength()*2);
			}

			DMZipItemArrayPtr pCurTheme=NULL;
			CStringW strName = lpszName;
			if (strName == m_strCurTheme// Ҫ���õĺ͵�ǰ����ͬ
				&&NULL != m_pCurTheme&&!m_bOutStyle)
			{
				iErr = DM_ECODE_OK;
				break;
			}

			pCurTheme = FindResItemObj(lpszName);
			if (NULL == pCurTheme)
			{
				break;
			}
			DMZipItemArrayPtr	pOldTheme = m_pCurTheme;  
			m_pCurTheme = pCurTheme;
			m_strCurTheme = strName;
			// 1.�Ƚ�����themes֮ǰ��index�Ĳ����ȫ��ͬ����Ϊ��Ҫ���£�����Ҫ���µ�type+name
			int nCount    = (int)m_pCurTheme->GetCount();
			int nCmpCount = (int)pOldTheme->GetCount();
			if (m_bOutStyle)
			{
				nCmpCount = 0;// �ⲿģʽ��Ӧ��Ҫ�������е�theme,����ֱ�ӰѱȽ�������Ϊ0
			}
			bool bFind = false;// �Ƿ����ǰthemesһ��
			CStringW strUpdateInfo;
			for (int i=0;i<nCount;i++)
			{
				DMZipItemPtr &pItem = m_pCurTheme->m_DMArray[i];
				for (int j=0;j<nCmpCount;j++)
				{
					bFind = false;// ��Ԥ��Ϊfalse
					DMZipItemPtr &pCmpItem = pOldTheme->m_DMArray[j];
					if (0 == _wcsicmp(pItem->m_szName,pCmpItem->m_szName)
						&&0 == _wcsicmp(pItem->m_szType,pCmpItem->m_szType))
					{
						if (0 == _wcsicmp(pItem->m_szPath,pCmpItem->m_szPath))
						{
							bFind = true;
						}
						break;// ������ѭ��
					}
				}
				if (false == bFind)
				{
					strUpdateInfo += pItem->m_szType;
					strUpdateInfo += L':';
					strUpdateInfo += pItem->m_szName;
					strUpdateInfo += L';';
				}
			}
			// TODO.��ʼ�㲥��Ϣ������Skin--
			int nLen = strUpdateInfo.GetLength()+1;
			wchar_t *pBuf = new wchar_t[nLen];
			memset(pBuf,0, nLen*2);
			memcpy(pBuf,strUpdateInfo.GetBuffer(),(nLen-1)*2);
			strUpdateInfo.ReleaseBuffer();
			g_pDMApp->UpdateSkin((WPARAM)pBuf,(LPARAM)nLen);
			DM_DELETE_ARRAY(pBuf);
			m_bOutStyle = false;// �������ⲿģʽ��
			iErr = DM_ECODE_OK;
		} while (false);
		return iErr;
	}

	DMCode DMResZipImpl::SendExpandInfo(WPARAM wp, LPARAM lp)
	{
		DMCode iErr = DM_ECODE_FAIL;
		do 
		{
			CStringW strAllTheme;
			int nCount = (int)GetCount();
			for (int i=0; i<nCount; i++)
			{
				DMZipItemArrayPtr &pCur = GetObj(i);
				CStringW strTheme = pCur->m_strThemeName;
				strTheme += L";";
				strAllTheme += strTheme;
			}
			int nSize = 2*(strAllTheme.GetLength()+1);
			if (wp&&NULL==lp)
			{
				if (!IsBadReadPtr((void*)wp,4))
				{
					*(DWORD*)wp  = nSize;
				}
			}
			else if(wp&&lp>=nSize)
			{
				if (!IsBadReadPtr((void*)wp,4))
				{
					memcpy((void*)wp,strAllTheme.GetBuffer(),nSize);
				}
			}
			iErr = DM_ECODE_OK;
		} while (false);
		return iErr;
	}

	// ����
	DMCode DMResZipImpl::ParseThemes(LPCWSTR lpszIndexPath)
	{
		DMCode iErr = DM_ECODE_FAIL;
		do 
		{
			if (NULL == lpszIndexPath)
			{
				break;
			}
			if (L'\\' == lpszIndexPath[0]||L'/'==lpszIndexPath[0])// ��ǰ�治�ܴ�\\����ȻZIP�޷�ʶ��
			{
				lpszIndexPath = lpszIndexPath+1;
			}
			CDMZipFile zf;
			if (!m_zipFile.GetFile(lpszIndexPath,zf))
			{
				break;
			}
			DMXmlDocument XmlDoc;
			if (false == XmlDoc.LoadFromBuffer(zf.GetData(),zf.GetSize()))
			{
				break;
			}
			DMXmlNode XmlNode = XmlDoc.Root();
			if (false == XmlNode.IsValid())
			{
				break;
			}

			XmlNode = XmlNode.FirstChild();
			while (XmlNode.IsValid())
			{
				InitDMData(XmlNode);// ʹ��xml�����滻
				DMXmlNode XmlFileNode = XmlNode.FirstChild(L"file");
				while (XmlFileNode.IsValid())
				{
					LPCWSTR lpszName = XmlFileNode.Attribute(L"name");
					LPCWSTR lpszFilePath = XmlFileNode.Attribute(L"path");
					wchar_t szPath[MAX_PATH] = {0};
					if (0 != lpszFilePath)
					{
						DMZipItemArrayPtr pItem = new DMZipItemArray;
						if (DMSUCCEEDED(ParseIndex(lpszFilePath,&pItem)))
						{
							if (m_strCurTheme.IsEmpty())
							{// Ϊ�գ���Ĭ�ϵ�һ��Ϊ��ǰʹ�õ�themes
								m_strCurTheme = lpszName;
								m_pCurTheme   = pItem;
							}
							pItem->m_strThemeName = lpszName;
							if (false == AddObj(pItem))
							{
								CStringW szInfo = lpszName;szInfo += L"������Ѵ���,�������ԣ�";DMASSERT_EXPR(0,szInfo);
								pItem->Release();
							}
						}
						else
						{
							pItem->Release();
						}
					}
					XmlFileNode = XmlFileNode.NextSibling(L"file");
				}
				XmlNode = XmlNode.NextSibling();
			}

			// ��ʼ����ǰʹ�õ�theme
			if (NULL == m_pCurTheme&&(int)GetCount()>=1)
			{
				m_pCurTheme = GetObj(0);
			}
			DMASSERT_EXPR(NULL!=m_pCurTheme,L"m_pCurTheme��ȻΪ�գ�������");
			iErr = DM_ECODE_OK;
		} while (false);
		return iErr;
	}

	DMCode DMResZipImpl::ParseIndex(LPCWSTR lpszIndexPath,DMZipItemArray** ppItem)
	{
		DMCode iErr = DM_ECODE_FAIL;
		do 
		{
			if (NULL == lpszIndexPath||NULL == ppItem)
			{
				break;
			}

			if (L'\\' == lpszIndexPath[0]||L'/'==lpszIndexPath[0])// ��ǰ�治�ܴ�\\����ȻZIP�޷�ʶ��
			{
				lpszIndexPath = lpszIndexPath+1;
			}

			CDMZipFile zf;
			if (!m_zipFile.GetFile(lpszIndexPath,zf))
			{
				break;
			}

			DMXmlDocument XmlDoc;
			if (false == XmlDoc.LoadFromBuffer(zf.GetData(),zf.GetSize()))
			{
				CStringW strInfo;strInfo.Format(L"xml LoadFromFile:%s fail",lpszIndexPath);DMASSERT_EXPR(0,strInfo);
				LOG_ERR("[mid]-xml LoadFromFile:%s fail\n",lpszIndexPath);
				break;
			}

			DMXmlNode XmlNode = XmlDoc.Root();
			if (false == XmlNode.IsValid())
			{
				LOG_ERR("[mid]-xmlnode inValid\n");
				break;
			}

			XmlNode = XmlNode.FirstChild();
			while (XmlNode.IsValid())
			{
				LPCWSTR lpszType = XmlNode.GetName();
				DMXmlNode XmlFileNode = XmlNode.FirstChild(L"file");
				while (XmlFileNode.IsValid())
				{
					LPCWSTR lpszName = XmlFileNode.Attribute(L"name");
					LPCWSTR lpszFilePath = XmlFileNode.Attribute(L"path");
					if (NULL!=lpszFilePath&&0!=wcslen(lpszFilePath))
					{
#if RES_INDEXFILE_CHECK
						CDMZipFile zftemp;
						if (!m_zipFile.GetFile(lpszFilePath,zftemp))
						{
							CStringW szInfo = lpszFilePath;szInfo+=L"�ļ������ڣ�";DMASSERT_EXPR(0,szInfo);
						}
						zftemp.Close();
#endif //RES_INDEXFILE_CHECK
						DMZipItem *pResItem = new DMZipItem(lpszType,lpszName,lpszFilePath);
						(*ppItem)->AddObj(pResItem);// ����DMArrayT
					}
					XmlFileNode = XmlFileNode.NextSibling(L"file");
				}

				XmlNode = XmlNode.NextSibling();
			}

			(*ppItem)->m_DMArray;//������DEBUG�鿴m_Array�ĵط�,watchһ�¾ͺ���
			iErr = DM_ECODE_OK;
		} while (false);
		return iErr;
	}

	LPCWSTR DMResZipImpl::GetItemPath(LPCWSTR lpszType,LPCWSTR lpszName,LPCWSTR lpszThemeName)
	{
		LPCWSTR lpszPath = NULL;
		do 
		{
			if (NULL == lpszType||NULL == lpszName)
			{
				break;
			}
			DMZipItem Item(lpszType, lpszName);// ����������EqualArrayObj��ֻҪǰ����������ȣ�����Ϊ�Ѵ���
			if (0 == _wcsicmp(lpszType, RES_LAYOUT)||0 == _wcsicmp(lpszType, RES_GLOBAL))
			{
				int iElement = m_pLayout->FindObj(&Item);
				if (DM_INVALID_VALUE == iElement)
				{
					break;
				}
				lpszPath = m_pLayout->GetObj(iElement)->m_szPath;
			}
			else
			{
				DMZipItemArrayPtr pTheme = FindResItemObj(lpszThemeName);
				if (NULL==pTheme)
				{
					break;
				}
				int iElement = pTheme->FindObj(&Item);
				if (DM_INVALID_VALUE == iElement)
				{
					if (false == m_bThemeLoop)
					{
						break;
					}
					bool bFind = false;// ������themes�в���
					int nCount = (int)GetCount();
					for (int i=0;i<nCount;i++)
					{
						DMZipItemArrayPtr &pCur = GetObj(i);
						iElement = pCur->FindObj(&Item);
						if (DM_INVALID_VALUE != iElement)
						{
							bFind = true;
							lpszPath = pCur->GetObj(iElement)->m_szPath;
							break;
						}
					}
					if (false == bFind)
					{
						break;
					}
				}
				else
				{
					lpszPath = pTheme->GetObj(iElement)->m_szPath;
				}
			}

		} while (false);
		return lpszPath;
	}

	DMZipItemArrayPtr DMResZipImpl::FindResItemObj(LPCWSTR lpszName)
	{
		DMZipItemArrayPtr pFindItem = NULL;
		do 
		{
			if (NULL == lpszName)
			{
				pFindItem = m_pCurTheme;
				break;
			}

			int nCount = (int)GetCount();
			for (int i=0; i<nCount; i++)
			{
				DMZipItemArrayPtr &pItem = GetObj(i);
				if (0==pItem->m_strThemeName.CompareNoCase(lpszName))
				{
					pFindItem = pItem;
					break;
				}
			}
		} while (false);
		return pFindItem;
	}

	void DMResZipImpl::PreArrayObjRemove(const DMZipItemArrayPtr &obj)
	{
		obj->Release(); // �����ü���Ϊ0�����Զ�delete this
	}

}//namespace DM
#pragma once

#include "nsDefine.h"

/*!
 * \brief
 * ����1��skin·��
 * ����2�������ڵ�xml id
 * ����3�������ڿؼ���
 *
 * \note
 * ��ʼ��GUIģ��
 *
 * \return �����ھ��
 */
NSISFUNC(NSInitSkinEngine);

/*!
 * \brief
 * ����1��control����
 *
 * \note
 * Ѱ��ָ����control
 *
 * \return �ҵ����ؿؼ���������򷵻�0
 */
NSISFUNC(NSFindControl);

/*!
 * \brief
 * ����1��control������
 * ����2���ص�������ַ
 *
 * \note
 * Ϊ�ؼ���֪ͨ��Ӧ����
 */
NSISFUNC(NSRegisterCommandNotify);

/*!
 * \brief
 * ����1��������xml id
 * ����2������ؼ�id
 * ����3����������
 * ����4�����ݿؼ�id
 * ����5��Э���ļ�·��
 *
 * \note
 * ��ʾЭ�鴰��
 */
NSISFUNC(NSShowLicense);

/*!
 * \brief
 * ����1������
 *
 * \note
 * ��ʾ�ļ�ѡ���
 * 
 * \return ���ȷ���������û�ѡ���·�������򷵻�0
 */
NSISFUNC(NSSelectFolderDialog);

/*!
 * \brief
 * ����1��control������
 * ����2������control������
 * ����3����������(1. DM��֧�ֵ�����; 2. ���֧�ֵ����� )
 *
 * \note
 * ���ÿؼ�����
 */
NSISFUNC(NSSetControlAttr);

/*!
 * \brief
 * ����1��control������
 * ����2�����ݵ����� (1. text; 2. check)
 *
 * \note
 * ��ȡ�ؼ�����
 *
 * \return �ؼ�����
 */
NSISFUNC(NSGetControlAttr);

/*!
 * \brief
 * ����1���������ڵ�����
 * ����2����������(1. DM��֧�ֵ�����; 2. ���֧�ֵ����� )
 *
 * \note
 * ��������������
 */
NSISFUNC(NSSetWindowAttr);

/*!
 * \brief
 * ����1�����ݵ�����
 *
 * \note
 * ��ȡ����������
 *
 * \return ����������
 */
NSISFUNC(NSGetWindowAttr);

/*!
 * \brief
 * ����1��TimerID
 * ����2��interval
 *
 * \note
 * ������ʱ��
 */
NSISFUNC(NSCreateTimer);

/*!
 * \brief
 * ����1��TimerID
 *
 * \note
 * ɾ����ʱ��
 */
NSISFUNC(NSKillTimer);

/*!
 * \brief
 * ����1����ϢID(�ַ���)
 * ����2��WPARAM
 * ����3��LPARAM
 *
 * \note
 * ����Ϣ
 */
NSISFUNC(NSSendMessage);

/*!
 * \brief
 * �޲���
 *
 * \note
 * ��ʾ���棬������Ϣѭ����ע�⣺һ��������Show������
 */
NSISFUNC(NSRun);

/*!
 * \brief
 * �޲���
 *
 * \note
 * �˳���װ
 */
NSISFUNC(NSUninitSkinEngine);

/*!
 * \brief
 * ����1��������xml id
 * ����2������ؼ�id
 * ����3����������
 * ����4�����ݿؼ�id
 * ����5������
 * ����6������(MBOK, MBOKCANCEL ...)
 *
 * \note
 * ����MessageBox
 *
 * \return �����û�����İ�ťID (IDOK, IDCANCEL, IDCLOSE, IDRETRY ...)
 */
NSISFUNC(NSMessageBox);

/*!
 * \brief
 * ����1��Ҫ�����ַ���
 *
 * \note
 * ����ַ����Ƿ�Ϸ���ֻ�ܰ�����[a-zA-Z0-9_]
 *
 * \return �����
 */
NSISFUNC(NSVerifyCharaters);

/*!
 * \brief
 * ����1��ҳ������
 *
 * \note
 * ��ת��ָ��ҳ��
 *
 */
NSISFUNC(NSSelectPage);

/*!
 * \brief
 *
 * \note
 * ��ת����һҳ(ҳ��˳����xml�ļ���ָ��)
 *
 */
NSISFUNC(NSPrevPage);

/*!
 * \brief
 *
 * \note
 * ��ת����һҳ(ҳ��˳����xml�ļ���ָ��)
 *
 */
NSISFUNC(NSNextPage);
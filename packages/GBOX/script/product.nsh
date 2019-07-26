!ifndef __PRODUCT_NSH__
!define __PRODUCT_NSH__

;��Ʒ����
;==================================================
!define    PRODUCT_NAME                  "GBOX"
!define    PRODUCT_VERSION               "2.0.0.28"
!define    PRODUCT_PUBLISHER             ""
!define    PRODUCT_WEB_SITE              ""
!define    PRODUCT_ROOT_KEY              "HKLM"
!define    PRODUCT_SUB_KEY               "SOFTWARE\GBOX"
!define    PRODUCT_UNINST_KEY            "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define    PRODUCT_UNINST_ROOT_KEY       "HKLM"
!define    PRODUCT_MAIN_EXE              "glauncher.exe"                                                      ;��װ��ɺ�������exe
!define    PRODUCT_UNINSTALL_NAME        "Uninstall.exe"                                                       ;ж�س�����
!define    PRODUCT_LEGAL                 "Copyright(C) 2007-2018 YY Inc. All Rights Reserved"
!define    PRODUCT_DESCRIPTION           "GBox�����"
!define    SETUP_MUTEX_NAME              "{E272344F-E832-4D90-9623-31ACA4B02EE4}"                              ;��ֹ�����ͬʱ����
!define    MUI_ICON                      "..\skin\install.ico"                                                 ;��װicon
!define    MUI_UNICON                    "..\skin\uninstall.ico"                                               ;ж��icon
!define    PRODUCT_CHANNEL_NAME          "official"                                                         ;�ֶ�������
!define    PRODUCT_CHANNEL_TAG           "23326"                                                           ;�ֶ˱�ʶ
!define    PRODUCT_CLIENT_EXE            "glauncher.exe;gbrowser.exe;gbmonitor32.exe;gbmonitor64.exe"                                      ;�ͻ��������������û���װ/ж��֮ǰ�������Ƿ�������
!define    PRODUCT_RSO_NAME              "${PRODUCT_CHANNEL_TAG}"                                                    ;pas�ֶ�rso
!define    PRODUCT_LNK_USER              "current"                                                                 ;��ݷ�ʽ��װ���û�[all, current]
!define    PRODUCT_DESKTOP_ARG           ''
!define    PRODUCT_STARTMENU_ARG         ''
!define    PRODUCT_TASKBAR_ARG           ''
!define    PRODUCT_INSTALL_SIZE          146800640                                                          ;��Ʒ��װ��Ҫռ�õĴ����ֽ���(140M)

Name       "${PRODUCT_NAME}"            ;��װ��������
OutFile    "gbox.exe"             ;��װ������
;InstallDir "$PROGRAMFILES\duowan\${PRODUCT_NAME}" ;Ĭ�ϰ�װλ��
InstallDir "$APPDATA\gbox" ;Ĭ�ϰ�װλ��

;==================================================
;PAS�ϱ�
!define PAS_ROOT_URL               "http://stat.game.yy.com/data.do"
!define PAS_SESSION_DATA           "act=/session_data"
!define PAS_EVENT_DATA             "act=/event"
!define PAS_PRODUCT_NAME           "gbox_v2"
;��װ�ϱ�
!define PAS_BEGIN_INSTALL          '"install/start"                  "��װ/��ʼ����"'          ;������װ����
!define PAS_INSTALL_DEFAULT        '"install/mode/default"           "��װ/ģʽ/Ĭ��"'         ;Ĭ�ϰ�װ
!define PAS_INSTALL_CUSTOM         '"install/mode/upgrade"           "��װ/ģʽ/����"'         ;�Զ��尲װ
!define PAS_INSTALL_WITH_TASKBAR   '"install/shortcut/taskbar"       "��װ/��ݷ�ʽ/������"'   ;����������ͼ��
!define PAS_INSTALL_WITH_STARTMENU '"install/shortcut/startmenu"     "��װ/��ݷ�ʽ/��ʼ�˵�"' ;������ʼ�˵�ͼ��
!define PAS_INSTALL_SUCCESS        '"install/success"                "��װ/�ɹ�"'              ;��װ�ɹ�
!define PAS_INSTALL_CHANNEL        '"install/channelfrom/${PRODUCT_CHANNEL_NAME}"                "��װ/����/������"'              ;��װ����
!define PAS_RUN_CLIENT             '"install/run_client"             "��װ/��������"'          ;�����ͻ���
!define PAS_INSTALL_FAILED         '"install/fail"                   "��װ/ʧ��"'              ;��װʧ��
!define PAS_INSTALL_FAILED_DESKSPACE         '"install/fail/1"                   "��װ/ʧ��/���̿ռ䲻��"'              ;��װʧ��
!define PAS_INSTALL_FAILED_UNKNOW         '"install/fail/-1"                   "��װ/ʧ��/δ֪����"'              ;��װʧ��
!define PAS_INSTALL_FAILED_TIMEOUT         '"install/fail/2"                   "��װ/ʧ��/�ȴ���ʱ"'              ;��װʧ��
!define PAS_INSTALL_FAILED_UNPACK         '"install/fail/3"                   "��װ/ʧ��/��ѹʧ��"'              ;��ѹʧ��
!define PAS_INSTALL_CANCELED       '"install/cancel"                 "��װ/ȡ��"'              ;ȡ����װ
!define PAS_END_INSTALL            '"install/end"                    "��װ/��������"'          ;�˳���װ����
;ж���ϱ�
!define PAS_BEGIN_UNINSTALL        '"uninstall/start"                "ж��/��ʼ����"'          ;����ж�س���
!define PAS_UNINSTALL_SUCCESS      '"uninstall/success"              "ж��/ж�سɹ�"'          ;ж�سɹ�
!define PAS_UNINSTALL_FAILED       '"uninstall/fail"                 "ж��/ж��ʧ��"'          ;ж��ʧ��
!define PAS_UNINSTALL_CANCELED     '"uninstall/cancel"               "ж��/ж��ȡ��"'          ;ȡ��ж��
!define PAS_END_UNINSTALL          '"uninstall/end"                  "ж��/��������"'          ;�˳�ж�س���
!define PAS_DOWNLOAD_CLIENT        '"click/4366game/downloadpage"    "���/������װ/����ҳ"'   ;�������ؿͻ���


!endif

!ifndef __PRODUCT_NSH__
!define __PRODUCT_NSH__

;��Ʒ����
;==================================================
!define    PRODUCT_NAME                  "YY��Ϸ����-���Ӱ�"
!define    PRODUCT_VERSION               "4.0.0.14"
!define    PRODUCT_PUBLISHER             "YY,Inc."
!define    PRODUCT_WEB_SITE              "http://lol.duowan.com/heziyouxi/"
!define    PRODUCT_ROOT_KEY              "HKLM"
!define    PRODUCT_SUB_KEY               "SOFTWARE\YY\YYGame"
!define    PRODUCT_UNINST_KEY            "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define    PRODUCT_UNINST_ROOT_KEY       "HKLM"
!define    PRODUCT_MAIN_EXE              "YGLauncher.exe"                                                      ;��װ��ɺ�������exe
!define    PRODUCT_UNINSTALL_NAME        "Uninstall.exe"                                                       ;ж�س�����
!define    PRODUCT_LEGAL                 "Copyright(C) 2007-2016 YY Inc. All Rights Reserved"
!define    PRODUCT_DESCRIPTION           "YY��Ϸ����-���Ӱ�"
!define    SETUP_MUTEX_NAME              "{A2E2A23D-8779-41F7-A5C6-220907073754}"                              ;��ֹ�����ͬʱ����
!define    MUI_ICON                      "..\skin\install.ico"                                                 ;��װicon
!define    MUI_UNICON                    "..\skin\uninstall.ico"                                               ;ж��icon
!define    PRODUCT_CHANNEL_NAME          "yygamesetup"                                                         ;�ֶ�������
!define    PRODUCT_CLIENT_EXE            "yygame.exe;yygrender.exe;yygupdate.exe;yygamecrash.exe;YGLauncher.exe;yg_server_reminder.exe"                                                          ;�ͻ��������������û���װ/ж��֮ǰ�������Ƿ�������
!define    PRODUCT_RSO_NAME              "from_lolgamesetup"                                                    ;pas�ֶ�rso
!define    PRODUCT_LNK_USER              "current"                                                                 ;��ݷ�ʽ��װ���û�[all, current]
!define    PRODUCT_DESKTOP_ARG           '"-scmd" "default" "from_desktop" "" "-oemid" "300"'
!define    PRODUCT_STARTMENU_ARG         '"-scmd" "default" "from_startmenu" "" "-oemid" "300"'
!define    PRODUCT_TASKBAR_ARG           '"-scmd" "default" "from_taskbar" "" "-oemid" "300"'

Name       "${PRODUCT_NAME}"            ;��װ��������
OutFile    "LOLGameSetup.exe"             ;��װ������
;InstallDir "$PROGRAMFILES\duowan\${PRODUCT_NAME}" ;Ĭ�ϰ�װλ��
InstallDir "$PROGRAMFILES\duowan\" ;Ĭ�ϰ�װλ��

;==================================================
;PAS�ϱ�
!define PAS_ROOT_URL               "http://stat.game.yy.com/data.do"
!define PAS_SESSION_DATA           "act=/session_data"
!define PAS_EVENT_DATA             "act=/event"
!define PAS_PRODUCT_NAME           "yygame"
;��װ�ϱ�
!define PAS_BEGIN_INSTALL          '"YYGameSetup/start"                  "������װ/��ʼ��װ"'                     ;������װ����
!define PAS_INSTALL_DEFAULT        '"YYGameSetup/start/default"          "������װ/��ʼ��װ/Ĭ��"'                ;Ĭ�ϰ�װ
!define PAS_INSTALL_CUSTOM         '"YYGameSetup/start/custom"           "������װ/��ʼ��װ/�Զ���"'              ;�Զ��尲װ
!define PAS_INSTALL_WITH_TASKBAR   '"YYGameSetup/start/custom/taskbar"   "������װ/��ʼ��װ/�Զ���/��������װ"'   ;����������ͼ��
!define PAS_INSTALL_WITH_STARTMENU '"YYGameSetup/start/custom/startmenu" "������װ/��ʼ��װ/�Զ���/��ʼ�˵���װ"' ;������ʼ�˵�ͼ��
!define PAS_INSTALL_SUCCESS        '"YYGameSetup/success"                "������װ/��װ�ɹ�"'                     ;��װ�ɹ�
!define PAS_RUN_CLIENT             '"YYGameSetup/success/startup"        "������װ/��װ�ɹ�/��������"'            ;�����ͻ���
!define PAS_INSTALL_FAILED         '"YYGameSetup/fail"                   "������װ/��װʧ��"'                     ;��װʧ��
!define PAS_INSTALL_CANCELED       '"YYGameSetup/cancel"                 "������װ/��װȡ��"'                     ;ȡ����װ
!define PAS_END_INSTALL            '"YYGameSetup/close"                  "������װ/�ر�"'                         ;�˳���װ����
;ж���ϱ�
!define PAS_BEGIN_UNINSTALL        '"YYGameSetup/uninstall/start"        "������װ/ж�س���/��ʼ"'                ;����ж�س���
!define PAS_UNINSTALL_SUCCESS      '"YYGameSetup/uninstall/success"      "������װ/ж�س���/ж�سɹ�"'            ;ж�سɹ�
!define PAS_UNINSTALL_FAILED       '"YYGameSetup/uninstall/fail"         "������װ/ж�س���/ж��ʧ��"'            ;ж��ʧ��
!define PAS_UNINSTALL_CANCELED     '"YYGameSetup/uninstall/cancel"       "������װ/ж�س���/ж��ȡ��"'            ;ȡ��ж��
!define PAS_END_UNINSTALL          '"YYGameSetup/uninstall/close"        "������װ/ж�س���/�ر�"'                ;�˳�ж�س���
!define PAS_DOWNLOAD_CLIENT        '"click/YYGameSetup/downloadpage"     "���/������װ/����ҳ"'                  ;�������ؿͻ���


!endif

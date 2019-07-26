!ifndef __PRODUCT_NSH__
!define __PRODUCT_NSH__

!define    PRODUCT_OEMID                "1101"
;产品配置
;==================================================
!define    PRODUCT_NAME                  "YY游戏大厅"
!define    PRODUCT_VERSION               "5.2.1.14"
!define    PRODUCT_PUBLISHER             "YY,Inc."
!define    PRODUCT_WEB_SITE              "http://wan.yy.com"
!define    PRODUCT_ROOT_KEY              "HKCU"
!define    PRODUCT_SUB_KEY               "SOFTWARE\YY\YYGame"
!define    PRODUCT_UNINST_KEY            "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define    PRODUCT_UNINST_ROOT_KEY       "HKCU"
!define    PRODUCT_MAIN_EXE              "YGLauncher.exe"                                                   ;安装完成后启动的exe
!define    PRODUCT_SCHEDULE_EXE          "BoxGameMsg.exe"                                                   	;安装完成后启动的计划任务进程
!define    PRODUCT_UNINSTALL_NAME        "Uninstall.exe"                                                       ;卸载程序名
!define    PRODUCT_LEGAL                 "Copyright(C) 2007-2018 YY Inc. All Rights Reserved"
!define    PRODUCT_DESCRIPTION           "YY游戏大厅安装版"
!define    SETUP_MUTEX_NAME              "{50A3E52E-6F7F-4411-9791-63BD15BBF2C2}"                              ;防止多进程同时运行
!define    MUI_ICON                      "..\skin\install.ico"                                                 ;安装icon
!define    MUI_UNICON                    "..\skin\uninstall.ico"                                               ;卸载icon
!define    PRODUCT_CHANNEL_NAME          "yygamesetup"                                                         ;分端渠道号
!define    PRODUCT_CLIENT_EXE            "yygame.exe;YGLauncher.exe;MountPlayer.exe;YGDownLoader.exe;DaemonService.exe;Update.exe"                                                          ;客户端主进程名，用户安装/卸载之前检测进程是否在运行
!define    PRODUCT_RSO_NAME              "from_yygamesetup"                                                    ;pas字段rso
!define    PRODUCT_LNK_USER              "current"                                                                 ;快捷方式安装的用户[all, current]
!define    PRODUCT_DESKTOP_ARG           '"-scmd" "default" "from_desktop" "" "-oemid" "${PRODUCT_OEMID}"'
!define    PRODUCT_STARTMENU_ARG         '"-scmd" "default" "from_startmenu" "" "-oemid" "${PRODUCT_OEMID}"'
!define    PRODUCT_TASKBAR_ARG           '"-scmd" "default" "from_taskbar" "" "-oemid" "${PRODUCT_OEMID}"'

Name       "${PRODUCT_NAME}"            ;安装程序名字
OutFile    "YYGameSetup.exe"             ;安装包名字
InstallDir "$LOCALAPPDATA\duowan\YYGame" ;默认安装位置

;==================================================
;PAS上报
!define PAS_ROOT_URL               "http://stat.game.yy.com/data.do"
!define PAS_SESSION_DATA           "act=/session_data"
!define PAS_EVENT_DATA             "act=/event"
!define PAS_PRODUCT_NAME           "yygame"
;安装上报
!define PAS_BEGIN_INSTALL          '"YYGameSetup/start"                  				"大厅安装/开始安装"'                     ;启动安装程序
!define PAS_INSTALL_DEFAULT        '"YYGameSetup/start/default"          				"大厅安装/开始安装/默认"'                ;默认安装
!define PAS_INSTALL_CUSTOM         '"YYGameSetup/start/custom"           				"大厅安装/开始安装/自定义"'              ;自定义安装
!define PAS_INSTALL_WITH_TASKBAR   '"YYGameSetup/start/custom/taskbar"   				"大厅安装/开始安装/自定义/任务栏安装"'   ;创建任务栏图标
!define PAS_INSTALL_WITH_STARTMENU '"YYGameSetup/start/custom/startmenu" 				"大厅安装/开始安装/自定义/开始菜单安装"' ;创建开始菜单图标
!define PAS_INSTALL_SUCCESS        '"YYGameSetup/success/${PRODUCT_OEMID}"			"大厅安装/安装成功/安装来源"'            ;安装成功
!define PAS_RUN_CLIENT             '"YYGameSetup/startup"        				 				"大厅安装/启动大厅"'            				;启动客户端
!define PAS_INSTALL_FAILED         '"YYGameSetup/fail"                   				"大厅安装/安装失败"'                     ;安装失败
!define PAS_INSTALL_CANCELED       '"YYGameSetup/cancel"                 				"大厅安装/安装取消"'                     ;取消安装
!define PAS_END_INSTALL            '"YYGameSetup/close"                  				"大厅安装/关闭"'                         ;退出安装程序
;卸载上报
!define PAS_BEGIN_UNINSTALL        '"YYGameSetup/uninstall/start"        							"大厅安装/卸载程序/开始"'                ;启动卸载程序
!define PAS_UNINSTALL_SUCCESS      '"YYGameSetup/uninstall/success/${PRODUCT_OEMID}" 	"大厅安装/卸载程序/卸载成功/安装来源"'   ;卸载成功
!define PAS_UNINSTALL_FAILED       '"YYGameSetup/uninstall/fail"         							"大厅安装/卸载程序/卸载失败"'            ;卸载失败
!define PAS_UNINSTALL_CANCELED     '"YYGameSetup/uninstall/cancel"       							"大厅安装/卸载程序/卸载取消"'            ;取消卸载
!define PAS_END_UNINSTALL          '"YYGameSetup/uninstall/close"        							"大厅安装/卸载程序/关闭"'                ;退出卸载程序
!define PAS_DOWNLOAD_CLIENT        '"click/YYGameSetup/downloadpage"     							"点击/大厅安装/下载页"'                  ;重新下载客户端


!endif

!ifndef __PRODUCT_NSH__
!define __PRODUCT_NSH__

;产品配置
;==================================================
!define    PRODUCT_NAME                  "4366游戏大厅"
!define    PRODUCT_VERSION               "1.0.0.21"
!define    PRODUCT_PUBLISHER             "YY,Inc."
!define    PRODUCT_WEB_SITE              "http://www.yy.com"
!define    PRODUCT_ROOT_KEY              "HKLM"
!define    PRODUCT_SUB_KEY               "SOFTWARE\YY\4366Game"
!define    PRODUCT_UNINST_KEY            "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define    PRODUCT_UNINST_ROOT_KEY       "HKLM"
!define    PRODUCT_MAIN_EXE              "Launcher.exe"                                                      ;安装完成后启动的exe
!define    PRODUCT_UNINSTALL_NAME        "Uninstall.exe"                                                       ;卸载程序名
!define    PRODUCT_LEGAL                 "Copyright(C) 2007-2017 YY Inc. All Rights Reserved"
!define    PRODUCT_DESCRIPTION           "4366游戏大厅客户端"
!define    SETUP_MUTEX_NAME              "{E272344F-E832-4D90-9623-31ACA4B02EE4}"                              ;防止多进程同时运行
!define    MUI_ICON                      "..\skin\install.ico"                                                 ;安装icon
!define    MUI_UNICON                    "..\skin\uninstall.ico"                                               ;卸载icon
!define    PRODUCT_CHANNEL_NAME          "qkzjggw"                                                         ;分端渠道号
!define    PRODUCT_CHANNEL_TAG           "23326"                                                           ;分端标识
!define    PRODUCT_CLIENT_EXE            "Launcher.exe"                                      ;客户端主进程名，用户安装/卸载之前检测进程是否在运行
!define    PRODUCT_RSO_NAME              "${PRODUCT_CHANNEL_TAG}"                                                    ;pas字段rso
!define    PRODUCT_LNK_USER              "current"                                                                 ;快捷方式安装的用户[all, current]
!define    PRODUCT_DESKTOP_ARG           ''
!define    PRODUCT_STARTMENU_ARG         ''
!define    PRODUCT_TASKBAR_ARG           ''
!define    PRODUCT_INSTALL_SIZE          12582912                                                          ;产品安装需要占用的磁盘字节数

Name       "${PRODUCT_NAME}"            ;安装程序名字
OutFile    "4366Game.exe"             ;安装包名字
;InstallDir "$PROGRAMFILES\duowan\${PRODUCT_NAME}" ;默认安装位置
InstallDir "$APPDATA\duowan\4366Game" ;默认安装位置

;==================================================
;PAS上报
!define PAS_ROOT_URL               "http://stat.game.yy.com/data.do"
!define PAS_SESSION_DATA           "act=/session_data"
!define PAS_EVENT_DATA             "act=/event"
!define PAS_PRODUCT_NAME           "4366game"
;安装上报
!define PAS_BEGIN_INSTALL          '"install/start"                  "安装/开始运行"'          ;启动安装程序
!define PAS_INSTALL_DEFAULT        '"install/mode/default"           "安装/模式/默认"'         ;默认安装
!define PAS_INSTALL_CUSTOM         '"install/mode/upgrade"           "安装/模式/升级"'         ;自定义安装
!define PAS_INSTALL_WITH_TASKBAR   '"install/shortcut/taskbar"       "安装/快捷方式/任务栏"'   ;创建任务栏图标
!define PAS_INSTALL_WITH_STARTMENU '"install/shortcut/startmenu"     "安装/快捷方式/开始菜单"' ;创建开始菜单图标
!define PAS_INSTALL_SUCCESS        '"install/success"                "安装/成功"'              ;安装成功
!define PAS_INSTALL_CHANNEL        '"install/channelfrom/${PRODUCT_CHANNEL_NAME}"                "安装/渠道/渠道号"'              ;安装渠道
!define PAS_RUN_CLIENT             '"install/run_client"             "安装/启动大厅"'          ;启动客户端
!define PAS_INSTALL_FAILED         '"install/fail"                   "安装/失败"'              ;安装失败
!define PAS_INSTALL_FAILED_DESKSPACE         '"install/fail/1"                   "安装/失败/磁盘空间不足"'              ;安装失败
!define PAS_INSTALL_FAILED_UNKNOW         '"install/fail/-1"                   "安装/失败/未知错误"'              ;安装失败
!define PAS_INSTALL_FAILED_TIMEOUT         '"install/fail/2"                   "安装/失败/等待超时"'              ;安装失败
!define PAS_INSTALL_FAILED_UNPACK         '"install/fail/3"                   "安装/失败/解压失败"'              ;解压失败
!define PAS_INSTALL_CANCELED       '"install/cancel"                 "安装/取消"'              ;取消安装
!define PAS_END_INSTALL            '"install/end"                    "安装/结束运行"'          ;退出安装程序
;卸载上报
!define PAS_BEGIN_UNINSTALL        '"uninstall/start"                "卸载/开始运行"'          ;启动卸载程序
!define PAS_UNINSTALL_SUCCESS      '"uninstall/success"              "卸载/卸载成功"'          ;卸载成功
!define PAS_UNINSTALL_FAILED       '"uninstall/fail"                 "卸载/卸载失败"'          ;卸载失败
!define PAS_UNINSTALL_CANCELED     '"uninstall/cancel"               "卸载/卸载取消"'          ;取消卸载
!define PAS_END_UNINSTALL          '"uninstall/end"                  "卸载/结束运行"'          ;退出卸载程序
!define PAS_DOWNLOAD_CLIENT        '"click/4366game/downloadpage"    "点击/大厅安装/下载页"'   ;重新下载客户端


!endif

# ====================== 安装程序初始定义常量 ==============================
; !define _DEBUG 1
SetCompressor lzma
!packhdr "$%TEMP%\yygame_exehead.tmp" '"..\..\..\script\upx.exe" -9 "$%TEMP%\yygame_exehead.tmp"'

# ====================== 产品自定义信息 ==============================
!include "product.nsh"

# ===================== 全局设置 =============================
!ifndef _DEBUG
	!verbose 3 ; 加快编译速度
!endif

# ===================== MUI设置 =============================
!include "MUI.nsh"

# ===================== 文件打包 =============================
ReserveFile "..\skin\skin.zip"
ReserveFile "..\addon\vcredist_x86.exe"
ReserveFile "..\addon\license.txt"
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS ; 安装预释放文件

# ===================== 引入第三方头文件和插件 =============================
!addincludedir "..\..\..\script"
!include "utils.nsh"
!include "nsdm.nsh"
!include "WinVer.nsh"
!include "StdUtils.nsh"
!include "nsCurl.nsh"
!ifdef _DEBUG
	!addplugindir "..\..\..\Debug"
!else
	!addplugindir "..\..\..\publish"
!endif

# ==================== NSIS属性 ================================
BrandingText /TRIMLEFT "${PRODUCT_PUBLISHER}"
SetFont "微软雅黑" 8
CRCCheck on
InstallDirRegKey ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "installDir" ; 从注册表获取安装路径
RequestExecutionLevel user ; 权限请求 none|user|highest|admin
Icon "..\skin\install.ico" ; 安装图标
UninstallIcon "..\skin\uninstall.ico" ; 卸载图标
ShowInstDetails nevershow ; 设置是否显示安装详细信息
ShowUninstDetails nevershow ; 设置是否显示卸载详细信息
!insertmacro MUI_LANGUAGE "SimpChinese"
;SilentUnInstall silent ; 静默安装
;SilentInstall silent ; 静默卸载

# ===================== 安装包版本 =============================
VIProductVersion                    "${PRODUCT_VERSION}"
VIAddVersionKey "ProductVersion"    "${PRODUCT_VERSION}"
VIAddVersionKey "ProductName"       "${PRODUCT_NAME}"
VIAddVersionKey "CompanyName"       "${PRODUCT_PUBLISHER}"
VIAddVersionKey "FileVersion"       "${PRODUCT_VERSION}"
VIAddVersionKey "InternalName"      "${PRODUCT_NAME}.exe"
VIAddVersionKey "FileDescription"   "${PRODUCT_DESCRIPTION}"
VIAddVersionKey "LegalCopyright"    "${PRODUCT_LEGAL}"

# ====================  Test =====================================

# ==================== 自定义页面 ================================
Page custom DMPage

# UninstPage custom un.DMPage

# ==================== 全局变量 ================================
!define DM_SKIN_PATH "$PLUGINSDIR\skin.zip"
!define GAME_LICENSE_PATH "$PLUGINSDIR\license.txt"
!define VC_REDIST_PATH "$PLUGINSDIR\vcredist_x86.exe"
!define NSDM_OUT $4
!define INSTALL_SIZE 12582912 ;"12 MB"
!define MB_OKCANCEL 1
!define MB_RETRYCANCEL 5
!define IDOK 1
!define IDCANCEL 2
!define IDRETRY 4
!define IDCLOSE 8
Var InstallState ; 安装状态(doing、done)
Var CountdownTimer ; 安装完成界面倒计时定时器
Var Countdown ; 倒计时计数
Var UninstallProgress ; 卸载进度
Var InstallRoot ; 安装根目录
Var PasSessionId ; pas上报session id
Var MBSize ; 弹出框大小
Var OverwriteInstall ; 是否覆盖安装: 0 未检测到已安装版本; 1 覆盖安装; 2 取消安装

Section "-Dummy"
	; 空白区段，防止编译器报错
	Nop
SectionEnd
# ==================== 安装区段 ================================
Function InstallFunc
	;复制要发布的安装文件
	SetOutPath "$INSTDIR"
	SetOverWrite on
	; File /r /x .svn "..\product\*.*"
	SetCompress off
	File "..\product\product.7z"
	SetCompress auto
	SetRebootFlag false

	GetFunctionAddress $R9 ExtractCallback
	Nsis7z::ExtractWithCallback "$OUTDIR\product.7z" $R9
	Delete "$OUTDIR\product.7z"
FunctionEnd

Function InstallFuncSlient
	;复制要发布的安装文件
	SetOutPath "$INSTDIR"
	SetOverWrite on
	; File /r /x .svn "..\product\*.*"
	SetCompress off
	File "..\product\product.7z"
	SetCompress auto
	SetRebootFlag false

	Nsis7z::Extract "$OUTDIR\product.7z"
	Delete "$OUTDIR\product.7z"
FunctionEnd


Function ExtractCallback
	Pop $R8 ; 已安装大小
  	Pop $R9 ; 总大小
  	System::Int64Op $R8 * 100
  	Pop $R7
  	System::Int64Op $R7 / $R9
  	Pop $R7 ; 安装百分比
  	${nsdm.SetControlAttr} "progress_installing" $R7 "value"

  	${If} $R8 == $R9
        ${nsdm.SetControlAttr} "progress_installing" 100 "value"
    ${EndIf}
FunctionEnd

; 生成快捷方式
Function BuildShortCut
	SetShellVarContext ${PRODUCT_LNK_USER}
  	CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" '${PRODUCT_DESKTOP_ARG}'

  	${nsdm.GetControlAttr} "btn_quick_taskbar" "check" $0
  	${If} $0 != "0"
  		Call CreateTaskbar
  	${EndIf}

  	${nsdm.GetControlAttr} "btn_startmenu_lnk" "check" $0
  	${If} $0 != "0"
  		Call CreateStartMenu
  	${EndIf}

  	${RefreshShellIcons}

  	SetShellVarContext current
FunctionEnd

; 创建开始菜单快捷方式
Function CreateStartMenu
	CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" '${PRODUCT_STARTMENU_ARG}'
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\卸载${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_UNINSTALL_NAME}"

	!insertmacro PostPasEvent ${PAS_INSTALL_WITH_STARTMENU}
FunctionEnd

; 创建任务栏图标
Function CreateTaskbar
    ${If} ${IsWin7}
    ${OrIf} ${IsWinVista}
    	; 删除已有图标
    	StrCpy $R1 "$QUICKLAUNCH\User Pinned\TaskBar\${PRODUCT_NAME}.lnk"
  		ExecShell "taskbarunpin" "$R1"
  		Delete "$R1"
  		; 创建新图标
    	StrCpy $R1 "$INSTDIR\${PRODUCT_NAME}.lnk"
  		CreateShortCut "$R1" "$INSTDIR\${PRODUCT_MAIN_EXE}" '${PRODUCT_TASKBAR_ARG}'
  		ExecShell "taskbarpin" "$R1"
  		Delete "$R1"
    ${ElseIf} ${AtLeastWin7}
    	; 删除已有图标
    	StrCpy $R1 "$QUICKLAUNCH\User Pinned\TaskBar"
        ${StdUtils.InvokeShellVerb} $0 "$R1" "${PRODUCT_NAME}.lnk" ${StdUtils.Const.ShellVerb.UnpinFromTaskbar}

    	; 使用插件创建任务栏图标
  		StrCpy $R1 "$INSTDIR\${PRODUCT_NAME}.lnk"
  		CreateShortCut "$R1" "$INSTDIR\${PRODUCT_MAIN_EXE}" '${PRODUCT_TASKBAR_ARG}'
        ${StdUtils.InvokeShellVerb} $0 "$INSTDIR" "${PRODUCT_NAME}.lnk" ${StdUtils.Const.ShellVerb.PinToTaskbar}
        !insertmacro "NSDM_DEBUG_INFO" "$0"
        Delete "$R1"
    ${Else}
    	IfFileExists "$QUICKLAUNCH\${PRODUCT_NAME}.lnk" 0 +2
    		Delete "$QUICKLAUNCH\${PRODUCT_NAME}.lnk"
    	CreateShortCut "$QUICKLAUNCH\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" '${PRODUCT_TASKBAR_ARG}'
  	${EndIf}
  	!insertmacro PostPasEvent ${PAS_INSTALL_WITH_TASKBAR}
FunctionEnd

; 生成卸载入口
Function BuildUninstaller
	;WriteUninstaller "$INSTDIR\${PRODUCT_UNINSTALL_NAME}"   ;Create uninstaller
	File "..\addon\${PRODUCT_UNINSTALL_NAME}"

	; 添加卸载信息到控制面板
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName"     "${PRODUCT_DESCRIPTION}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\${PRODUCT_UNINSTALL_NAME}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon"     "$INSTDIR\${PRODUCT_MAIN_EXE}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher"       "${PRODUCT_PUBLISHER}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "InstallLocation" "$INSTDIR"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion"  "${PRODUCT_VERSION}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "HelpLink"        "${PRODUCT_WEB_SITE}"

	${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
 	IntFmt $0 "0x%08X" $0
	WriteRegDWORD ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "EstimatedSize" "$0"
FunctionEnd


Function SlientInstall
	!insertmacro "NSDM_DEBUG_INFO" "进入静默安装"

	!insertmacro PostPasEvent ${PAS_BEGIN_INSTALL}

	; 检查是否已经安装。已有版本，无论新旧直接退出

	ReadRegStr $1 ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "version"
	${If} "$1" != ""
		!insertmacro "NSDM_DEBUG_INFO" "已经安装过，退出"
		Goto WaitThread_PostPasEvent
	${EndIf}

	; 兼容旧版本
	ReadRegStr $1 "HKLM" "${PRODUCT_SUB_KEY}" "version"
	${If} "$1" != ""
		!insertmacro "NSDM_DEBUG_INFO" "已经安装过，退出"
		Goto WaitThread_PostPasEvent
	${EndIf}

	!insertmacro "NSDM_DEBUG_INFO" "SetLastInstallPath INSTDIR $INSTDIR"
	Call SetLastInstallPath
	!insertmacro "NSDM_DEBUG_INFO" "SetLastInstallPath INSTDIR $INSTDIR"
	
	; 检查磁盘空间是否足够
	!insertmacro "GetFreeSpaceSize" "$INSTDIR" $R0
	System::Int64Op $R0 > ${INSTALL_SIZE}
	Pop $0
	${If} $0 == 0
		!insertmacro "NSDM_DEBUG_INFO" "磁盘空间不足，退出"
		Goto WaitThread_PostPasEvent
	${EndIf}

	; 检查客户端进程是否在运行
	!insertmacro "ProcessDetect" "${PRODUCT_CLIENT_EXE}" ";"
	Pop $0
	${If} $0 != 0
		!insertmacro "NSDM_DEBUG_INFO" "已经有一个安装进程实例，退出"
		Goto WaitThread_PostPasEvent
	${EndIf}

	SetRegView 32
	; 检测vcredist
	Call CheckVCRedist
	IfErrors 0 Install_Continue
		!insertmacro "NSDM_DEBUG_INFO" "当前系统没有检测到vc2008 redist"
		;GetFunctionAddress $0 InstallVCRedist
    ;BgWorker::CallAndWait
		Call InstallVCRedist
		IfErrors 0 Install_Continue
			Call HandleVCRedistCancel
			!insertmacro "NSDM_DEBUG_INFO" "安装vc2008 redist失败，即将退出"
			Goto WaitThread_PostPasEvent
 	Install_Continue:
  		!insertmacro "NSDM_DEBUG_INFO" "已经安装vc2008 redist"

	; 写入注册表信息
	WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "installDir" "$INSTDIR"
	WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "version" "${PRODUCT_VERSION}"

 	!insertmacro "NSDM_DEBUG_INFO" "解压文件到指定目录"
  Call InstallFuncSlient

	SetShellVarContext ${PRODUCT_LNK_USER}

	Push "/nodeskicon"
	Call ParameterDetect
	Pop $0
			!insertmacro "NSDM_DEBUG_INFO" "$0"
	${If} $0 == 0
		!insertmacro "NSDM_DEBUG_INFO" "创建桌面快捷方式"
	  CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" '${PRODUCT_DESKTOP_ARG}'
	${Else}
		!insertmacro "NSDM_DEBUG_INFO" "检测到启动参数/nodeskicon，不创建桌面快捷方式"
	${EndIf}

	Push "/notaskbaricon"
	Call ParameterDetect
	Pop $0
		!insertmacro "NSDM_DEBUG_INFO" "$0"
	${If} $0 == 0
		!insertmacro "NSDM_DEBUG_INFO" "创建任务栏快捷方式"
  	Call CreateTaskbar
	${Else}
		!insertmacro "NSDM_DEBUG_INFO" "检测到启动参数/notaskbaricon，不创建任务栏快捷方式"
	${EndIf}


  !insertmacro "NSDM_DEBUG_INFO" "创建开始菜单快捷方式"
  Call CreateStartMenu

  ${RefreshShellIcons}
  SetShellVarContext current

	Call BuildUninstaller

	StrCpy $InstallState "done"
	!insertmacro PostPasEvent ${PAS_INSTALL_SUCCESS}

	;!insertmacro "NSDM_DEBUG_INFO" "启动游戏大厅"
	;Exec '"$INSTDIR\${PRODUCT_MAIN_EXE}" ${PRODUCT_DESKTOP_ARG}'
	;!insertmacro PostPasEvent ${PAS_RUN_CLIENT}
	
	; 启动计划任务进程
	; Exec '"$INSTDIR\${PRODUCT_SCHEDULE_EXE}" ""'
	nsutil::NSUpdateSchedule "0EDF91F7-D4D0-4DD7-B739-62067F6683FC" "YY游戏大厅更新任务" "$INSTDIR\popup" "$INSTDIR\popup\${PRODUCT_SCHEDULE_EXE}" "--type=yyGameOem" "60"


	; 安装结束，数据上报和等待上报结束
WaitThread_PostPasEvent:
	!insertmacro "NSDM_DEBUG_INFO" "安装结束，启动数据上报"

	!insertmacro PostSessionData
	${nscurl.WaitDone} 10000 $0

	!insertmacro "NSDM_DEBUG_INFO" "安装结束，数据上报结束"
FunctionEnd

Function ParameterDetect
	Pop $0
	ClearErrors
	${GetParameters} $1
	${GetOptions} $1 "$0" $2
	IfErrors 0 ParamExist
			!insertmacro "NSDM_DEBUG_INFO" "Push 0"
		Push 0
		Return
 	ParamExist:
 			!insertmacro "NSDM_DEBUG_INFO" "Push 1"
		Push 1
		Return
FunctionEnd

Function GUIInstall
	StrCpy $InstallState "doing"

	!insertmacro NSDM_DEBUG_INFO "初始化DM库"
	${nsdm.InitSkinEngine} ${DM_SKIN_PATH} "dui_install_wnd" "install_frame" ${NSDM_OUT}

	Call BindUIControls

	Call InitDialog

	Call CheckInstalled

	${If} $OverwriteInstall == 2
		!insertmacro NSDM_DEBUG_INFO "检测到已安装版本，用户取消安装，未进入消息循环，直接退出"
	${Else}
		!insertmacro NSDM_DEBUG_INFO "覆盖安装，进入消息循环"
		${nsdm.Run}
	${EndIf}
	!insertmacro NSDM_DEBUG_INFO "卸载DM库"

	!insertmacro PostSessionData
	${nscurl.WaitDone} 10000 $0
FunctionEnd

Function DMPage
	!insertmacro NSDM_DEBUG_INFO "非静默安装"
	Call GUIInstall
FunctionEnd


Function InitDialog
	StrCpy ${NSDM_OUT} $INSTDIR
	Call SetInstallFolder

	StrCpy $MBSize "586,436"

	${nsdm.SetControlAttr} "btn_checkbox_license" 1 "check"
	${nsdm.SetControlAttr} "btn_quick_taskbar" 1 "check"
	${nsdm.SetControlAttr} "btn_startmenu_lnk" 1 "check"

	${nsdm.SetControlAttr} "label_install_title" "${PRODUCT_DESCRIPTION}${PRODUCT_VERSION}" "text"

	!insertmacro PostPasEvent ${PAS_BEGIN_INSTALL}
FunctionEnd

; 检测是否已经安装客户端
Function CheckInstalled
	ReadRegStr $1 ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "version"
	StrCpy $OverwriteInstall 0
	${If} "$1" == ""
		; 兼容旧版本
		ReadRegStr $1 "HKLM" "${PRODUCT_SUB_KEY}" "version"
		${If} "$1" == ""
			Return
		${EndIf}
	${EndIf}

	MessageBox MB_YESNO|MB_ICONQUESTION "检测到已经安装${PRODUCT_NAME} $1, 是否覆盖安装？" IDYES yes IDNO no
	yes:
		; ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "installDir"
		; 禁止用户修改安装路径
		${nsdm.SetControlAttr} "edit_install_folder" "1" "bdisable"
		${nsdm.SetControlAttr} "btn_explorer" "1" "bdisable"
		StrCpy $OverwriteInstall 1
		Return
	no:
		StrCpy $InstallState "done"
		Call OnExitDUISetup
		StrCpy $OverwriteInstall 2
		Return
FunctionEnd

Function .onInit
	; 检查安装进程是否在运行
	!insertmacro "CheckSetupRunning" ${SETUP_MUTEX_NAME} ${NSDM_OUT}
	${If} ${NSDM_OUT} == 1
		!insertmacro "NSDM_DEBUG_INFO" "安装或卸载程序已经在运行，退出"
		IfSilent 0 MsgBoxAndAbort
			Abort
		MsgBoxAndAbort:
			MessageBox MB_OK|MB_ICONEXCLAMATION "安装或卸载程序已经在运行。"
   		Abort
	${EndIf}

	!insertmacro "NSDM_DEBUG_INFO" ".onInit"
	InitPluginsDir
	File "/ONAME=${DM_SKIN_PATH}" "..\skin\skin.zip"
	File "/ONAME=${GAME_LICENSE_PATH}" "..\addon\license.txt"
	File "/ONAME=${VC_REDIST_PATH}" "..\addon\vcredist_x86.exe"

	!insertmacro SetSessionId
	!insertmacro PostSessionData

	IfSilent 0 NonSlientInstall
		!insertmacro NSDM_DEBUG_INFO "静默安装"
		Call SlientInstall
	NonSlientInstall:
FunctionEnd

; 安装界面点击退出，给出提示
Function OnExitDUISetup
	!insertmacro "NSDM_DEBUG_INFO" "$InstallState"
	${If} $InstallState == "doing"
		${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "提示" "label_popup_content" "安装尚未完成，您确定退出安装么？" ${MB_OKCANCEL} ${NSDM_OUT}
		!insertmacro "NSDM_DEBUG_INFO" "${NSDM_OUT}"
		${If} ${NSDM_OUT} != ${IDOK}
			Return
		${Else}
			!insertmacro PostPasEvent ${PAS_INSTALL_CANCELED}
		${EndIf}
	${EndIf}
	${nsdm.UninitSkinEngine}
	!insertmacro PostPasEvent ${PAS_END_INSTALL}
FunctionEnd

Function OnBtnClickLicense
	${nsdm.ShowLicense} "dui_license_wnd" "label_install_title" "${PRODUCT_DESCRIPTION}" "edit_install_license" ${GAME_LICENSE_PATH} ${NSDM_OUT}
FunctionEnd

Function OnBtnSelectPath
	${nsdm.SelectFolderDialog} "${PRODUCT_DESCRIPTION}" ${NSDM_OUT}
	!insertmacro NSDM_DEBUG_INFO "${NSDM_OUT}"
	Call SetInstallFolder
FunctionEnd

Function OnBtnStartGame
	!insertmacro "NSDM_DEBUG_INFO" "OnBtnStartGame"
	Exec '"$INSTDIR\${PRODUCT_MAIN_EXE}" ${PRODUCT_DESKTOP_ARG}'
		; 启动计划任务进程
	; Exec '"$INSTDIR\${PRODUCT_SCHEDULE_EXE}" ""'
	nsutil::NSUpdateSchedule "0EDF91F7-D4D0-4DD7-B739-62067F6683FC" "YY游戏大厅更新任务" "$INSTDIR\popup" "$INSTDIR\popup\${PRODUCT_SCHEDULE_EXE}" "--type=yyGameOem" "60"
	!insertmacro PostPasEvent ${PAS_RUN_CLIENT}
	Call OnExitDUISetup
FunctionEnd

Function SetInstallFolder
	!insertmacro "NSDM_DEBUG_INFO" "SetInstallFolder: ${NSDM_OUT}"
	${If} "${NSDM_OUT}" != "0"
	${AndIf} "${NSDM_OUT}" != ""
		;${If} $OverwriteInstall == 0
		;	; 非覆盖安装模式下，才允许调整安装路径
		;	nsutil::NSCreateSeparatePath "${NSDM_OUT}" "${PRODUCT_NAME}"
		;	Pop ${NSDM_OUT}
		;${EndIf}
		StrCpy $INSTDIR "${NSDM_OUT}"
		${GetRoot} "${NSDM_OUT}" $InstallRoot

		${nsdm.SetControlAttr} "edit_install_folder" "${NSDM_OUT}" "text"

		!insertmacro "FormatSizeString" ${INSTALL_SIZE} $R0
		StrCpy $R1 "所需空间: $R0"
		${nsdm.SetControlAttr} "label_install_size" $R1 "text"

		!insertmacro "GetFreeSpaceSize" "${NSDM_OUT}" $R0
		!insertmacro "FormatSizeString" $R0 $R0
		StrCpy $R1 "磁盘可用空间: $R0"
		${nsdm.SetControlAttr} "label_useable_size" $R1 "text"

	${EndIf}
FunctionEnd

Function CheckVCRedist
	ClearErrors

	nsutil::NSIsVC2008Installed_x86
	Pop $R0
	nsutil::NSIsVC2008SP1Installed_x86
	Pop $R1
	${If} $R0 == 0
	${AndIf} $R1 == 0
		SetErrors
	${EndIf}
FunctionEnd

Function InstallVCRedist
	ClearErrors
	ExecWait "${VC_REDIST_PATH}" $0
	!insertmacro "NSDM_DEBUG_INFO" "$0"
	${If} $0 != 0
		SetErrors
	${EndIf}
FunctionEnd

Function HandleVCRedistCancel
	${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "错误提示" "label_popup_content" "当前环境无法安装${PRODUCT_NAME}，安装程序即将退出！" ${MB_OKCANCEL} $0
	StrCpy $InstallState "done"
	Call OnExitDUISetup
FunctionEnd

#设置最终安装路径
Function SetLastInstallPath
	StrCpy $INSTDIR "$INSTDIR\${PRODUCT_NAME}"
FunctionEnd

Function OnBtnClickInstall
	; 确保安装路径为用户最终选择路径
	${nsdm.GetControlAttr} "edit_install_folder" "text" ${NSDM_OUT}
	Call SetInstallFolder

	;生成最终的安装路径
	${If} $OverwriteInstall == 0
		; 非覆盖安装模式下，才设置最终路径
		Call SetLastInstallPath
	${EndIf}


	; 检查磁盘空间是否足够
	!insertmacro "GetFreeSpaceSize" "${NSDM_OUT}" $R0
	System::Int64Op $R0 > ${INSTALL_SIZE}
	Pop $0
	${If} $0 == 0
		${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "错误提示" "label_popup_content" "磁盘空间不足，请选择其他磁盘!" ${MB_OKCANCEL} $0
		Return
	${EndIf}

	; 检查客户端进程是否在运行
	Push "${PRODUCT_CLIENT_EXE}"
	Call TerminateIfExists
	Pop $0
	${If} $0 == 0
		Return
	${EndIf}

	; 隐藏扩展窗口
	Call HideCustomWnd

	; 判断是否自定义安装
	${nsdm.GetControlAttr} "edit_install_folder" "text" ${NSDM_OUT}
	StrCmp "$INSTDIR" "${NSDM_OUT}" 0 begin_custom_install
	!insertmacro PostPasEvent ${PAS_INSTALL_DEFAULT}
	Goto end_custom_install
 begin_custom_install:
	!insertmacro PostPasEvent ${PAS_INSTALL_CUSTOM}
 end_custom_install:

	${nsdm.SelectPage} "wnd_installling"

	SetRegView 32
	; 检测vcredist
	Call CheckVCRedist
	IfErrors 0 Install_Continue
	!insertmacro "NSDM_DEBUG_INFO" "当前系统没有检测到vc2008 redist"
	GetFunctionAddress $0 InstallVCRedist
    BgWorker::CallAndWait
	IfErrors 0 Install_Continue
	Call HandleVCRedistCancel
	!insertmacro "NSDM_DEBUG_INFO" "安装vc2008 redist失败，即将退出"
	Return
 Install_Continue:
 	!insertmacro "NSDM_DEBUG_INFO" "已经安装vc2008 redist"

	; 写入注册表信息
	WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "installDir" "$INSTDIR"
	WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "version" "${PRODUCT_VERSION}"

	${nsdm.SetControlAttr} "btn_close" 1 "bdisable" ; 释放文件中禁止退出
	; 启动一个低优先级的后台线程
    GetFunctionAddress $0 InstallFunc
    BgWorker::CallAndWait
    ${nsdm.SetControlAttr} "btn_close" 0 "bdisable"

	${nsdm.SetControlAttr} "label_counter_start" "3 s后自动体验游戏大厅" "text"
	${nsdm.SelectPage} "wnd_installed"

	StrCpy $Countdown 3
	GetFunctionAddress $CountdownTimer OnCountdownTimer
	${nsdm.CreateTimer} $CountdownTimer 1000

	Call BuildShortCut
	Call BuildUninstaller

	StrCpy $InstallState "done"
	!insertmacro PostPasEvent ${PAS_INSTALL_SUCCESS}
FunctionEnd

; 自定义安装checkbox
Function OnBtnCustomInstall
	${nsdm.GetControlAttr} "btn_checkbox_custom_install" "check" $0
	${If} $0 == 0
		Call HideCustomWnd
	${Else}
		Call ShowCustomWnd
	${EndIf}
FunctionEnd

Function HideCustomWnd
	StrCpy $MBSize "586,436"
	${nsdm.SetWindowAttr} "$MBSize" "possize"

FunctionEnd

Function ShowCustomWnd
	StrCpy $MBSize "586,580"
	${nsdm.SetWindowAttr} "$MBSize" "possize"
FunctionEnd

; 协议checkbox
Function OnBtnCheckLicense
	${nsdm.GetControlAttr} "btn_checkbox_license" "check" $0
	${If} $0 == 0
		${nsdm.SetControlAttr} "btn_install"         "1" "bdisable"
		${If} $OverwriteInstall == 0
			${nsdm.SetControlAttr} "btn_explorer"        "1" "bdisable"
			${nsdm.SetControlAttr} "edit_install_folder" "1" "bdisable"
		${EndIf}

		${nsdm.SetControlAttr} "btn_quick_taskbar"   "1" "bdisable"
		${nsdm.SetControlAttr} "btn_startmenu_lnk"   "1" "bdisable"
		; ${nsdm.SetControlAttr} "label_custom_install" "rgba(89,a9,bb,ff)" "clrtext"
		${nsdm.SetControlAttr} "btn_checkbox_custom_install"   "1" "bdisable"
		${nsdm.SetControlAttr} "btn_checkbox_custom_install" "rgba(89,a9,bb,ff)" "clrtext"
	${Else}
		${nsdm.SetControlAttr} "btn_install"         "0" "bdisable"
		${If} $OverwriteInstall == 0
			${nsdm.SetControlAttr} "btn_explorer"        "0" "bdisable"
			${nsdm.SetControlAttr} "edit_install_folder" "0" "bdisable"
		${EndIf}

		${nsdm.SetControlAttr} "btn_quick_taskbar"   "0" "bdisable"
		${nsdm.SetControlAttr} "btn_startmenu_lnk"   "0" "bdisable"
		; ${nsdm.SetControlAttr} "label_custom_install" "rgba(30,9b,df,ff)" "clrtext"
		${nsdm.SetControlAttr} "btn_checkbox_custom_install"   "0" "bdisable"
		${nsdm.SetControlAttr} "btn_checkbox_custom_install" "rgba(30,9b,df,ff)" "clrtext"
	${EndIf}
FunctionEnd

; 当edit控件内容改变时调用
Function OnPathEditChange
	${nsdm.GetControlAttr} "edit_install_folder" "text" ${NSDM_OUT}
	Push $0
	${GetRoot} "${NSDM_OUT}" $0
	StrCmp "$InstallRoot" "$0" +2 0
	Call SetInstallFolder
	Pop $0
FunctionEnd

Function OnBtnRetry
	!insertmacro "NSDM_DEBUG_INFO" "OnBtnRetry"
FunctionEnd

; 绑定安装界面事件
Function BindUIControls
	; 全局
	GetFunctionAddress $0 "OnExitDUISetup"
	${nsdm.RegisterCommandNotify} "btn_close" $0

	; 主页
	GetFunctionAddress $0 "OnBtnClickLicense"
	${nsdm.RegisterCommandNotify} "btn_license" $0

	GetFunctionAddress $0 "OnBtnSelectPath"
	${nsdm.RegisterCommandNotify} "btn_explorer" $0

	GetFunctionAddress $0 "OnBtnClickInstall"
	${nsdm.RegisterCommandNotify} "btn_install" $0

	GetFunctionAddress $0 "OnBtnCustomInstall"
	${nsdm.RegisterCommandNotify} "btn_checkbox_custom_install" $0

	GetFunctionAddress $0 "OnBtnCheckLicense"
	${nsdm.RegisterCommandNotify} "btn_checkbox_license" $0

	GetFunctionAddress $0 "OnPathEditChange"
	${nsdm.RegisterCommandNotify} "edit_install_folder" $0

	GetFunctionAddress $0 "OnBtnRetry"
	${nsdm.RegisterCommandNotify} "btn_retry" $0

	; 安装完成页面
	GetFunctionAddress $0 "OnBtnStartGame"
	${nsdm.RegisterCommandNotify} "btn_start_client" $0
FunctionEnd

Function OnCountdownTimer
	!insertmacro "NSDM_DEBUG_INFO" "启动游戏倒计时: $Countdown"

	IntOp $Countdown $Countdown - 1
	${If} $Countdown == 0
		${nsdm.KillTimer} $CountdownTimer
		Call OnBtnStartGame
	${Else}
		StrCpy $0 "$Countdown s后自动体验游戏大厅"
		${nsdm.SetControlAttr} "label_counter_start" "$0" "text"
	${EndIf}
FunctionEnd

Function TerminateIfExists
	Pop $R2 ; 进程名集合

	!insertmacro "ProcessDetect" "$R2" ";"
	Pop $0
	${If} $0 == 0
		Push 1
		Return
	${EndIf}

	${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "提示" "label_popup_content" "检测到${PRODUCT_NAME}正在运行，点击[确定]强制结束进程！" ${MB_OKCANCEL} $0
	${If} $0 == ${IDOK}
		${Do}
			!insertmacro "ExeEnd" "$R2" ";"
			Pop $1
			!insertmacro "NSDM_DEBUG_INFO" "ExeEnd result $1"
			${If} $1 == -1
				${Break}
			${EndIf}
		${LoopWhile} $1 == 1

		${If} $1 == -1
			${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "提示" "label_popup_content" "检测到${PRODUCT_NAME}正在运行，无法强制结束进程！" ${MB_OKCANCEL} $0
			Push 0
		${Else}
			Push 1
		${EndIf}
	${Else}
		Push 0
	${EndIf}
FunctionEnd



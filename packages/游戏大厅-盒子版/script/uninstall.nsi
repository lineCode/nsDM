# ====================== 安装程序初始定义常量 ==============================
#!define _DEBUG 1
;SetCompressor lzma
SetCompress off
!packhdr "$%TEMP%\yygame_exehead.tmp" '"..\..\..\script\upx.exe" -9 "$%TEMP%\yygame_exehead.tmp"'

# ====================== 产品自定义信息 ==============================
!include "product.nsh"

# ===================== 全局设置 =============================
!ifndef _DEBUG
	!verbose 3 ; 加快编译速度
!endif

# ===================== MUI设置 =============================
!include "MUI.nsh"


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
RequestExecutionLevel admin ; 权限请求 none|user|highest|admin
Icon "..\skin\install.ico" ; 安装图标
UninstallIcon "..\skin\uninstall.ico" ; 卸载图标
ShowInstDetails hide ; 设置是否显示安装详细信息nevershow
ShowUninstDetails hide ; 设置是否显示卸载详细信息nevershow
!insertmacro MUI_LANGUAGE "SimpChinese"
SilentInstall silent ; 静默安装
;SilentUnInstall silent ; 静默卸载


# ===================== 安装包版本 =============================
VIProductVersion                    "${PRODUCT_VERSION}"
VIAddVersionKey "ProductVersion"    "${PRODUCT_VERSION}"
VIAddVersionKey "ProductName"       "${PRODUCT_NAME}"
VIAddVersionKey "CompanyName"       "${PRODUCT_PUBLISHER}"
VIAddVersionKey "FileVersion"       "${PRODUCT_VERSION}"
VIAddVersionKey "InternalName"      "${PRODUCT_NAME}.exe"
VIAddVersionKey "FileDescription"   "${PRODUCT_DESCRIPTION}"
VIAddVersionKey "LegalCopyright"    "${PRODUCT_LEGAL}"

# ==================== 自定义页面 ================================

UninstPage custom un.DMPage

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
;Var CountdownTimer ; 安装完成界面倒计时定时器
;Var Countdown ; 倒计时计数
Var UninstallProgress ; 卸载进度
;Var InstallRoot ; 安装根目录
Var PasSessionId ; pas上报session id
Var MBSize ; 弹出框大小
;Var OverwriteInstall ; 是否覆盖安装: 0 未检测到已安装版本; 1 覆盖安装; 2 取消安装

OutFile    "uninst_build.exe"             ;安装包名字

;!finalize 'call "%1" & del "%1"'          ;执行生成的uninst_build.exe
	
Section "-Dummy"
	; 生成uninstall.exe文件，并放置到当前目录下
	WriteUninstaller "$EXEDIR\..\addon\${PRODUCT_UNINSTALL_NAME}"
	;Nop
SectionEnd


# ==================== 卸载区段 ================================
Function un.UninstallFunc
	; 检查客户端进程是否在运行
	!insertmacro "NSDM_DEBUG_INFO" "${PRODUCT_CLIENT_EXE}"
	Push "${PRODUCT_CLIENT_EXE}"
	Call un.TerminateIfExists
	Pop $0
	${If} $0 == 0
		Return
	${ElseIf} $0 == -1
		${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "提示" "label_popup_content" "无法强制结束'${PRODUCT_CLIENT_EXE}'，请手动退出！" ${MB_OKCANCEL} $R0
		Return
	${EndIf}

	!insertmacro "NSDM_DEBUG_INFO" "开始卸载"
	${nsdm.SetControlAttr} "btn_close" 1 "bdisable"
	${nsdm.SelectPage} "wnd_uninstallling"

	; 删除注册表信息
	SetRegView 32
	DeleteRegKey ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}"
	DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
	IntOp $UninstallProgress 0 + 1
	${nsdm.SetControlAttr} "progress_uninstalling" $UninstallProgress "value"

	; 删除快捷方式
	Call un.DeleteShortCut
	IntOp $UninstallProgress $UninstallProgress + 2
	${nsdm.SetControlAttr} "progress_uninstalling" $UninstallProgress "value"

	; 后台删除文件
	GetFunctionAddress $0 un.RemoveFiles
	BgWorker::CallAndWait

	${nsdm.SelectPage} "wnd_uninstalled"
	
	Delete "$INSTDIR\${PRODUCT_UNINSTALL_NAME}"
	; RMDir /r "$INSTDIR"
	RMDir "$INSTDIR" ; 非空才删除

	!insertmacro PostPasEvent ${PAS_UNINSTALL_SUCCESS}
FunctionEnd

Function un.DeleteShortCut
	SetShellVarContext ${PRODUCT_LNK_USER}
	${If} ${IsWin7}
    ${OrIf} ${IsWinVista}
    	StrCpy $R1 "$QUICKLAUNCH\User Pinned\TaskBar\${PRODUCT_NAME}.lnk"
  		ExecShell "taskbarunpin" "$R1"
  		Delete "$R1"
	${ElseIf} ${AtLeastWin7}
        ; 使用插件删除任务栏图标，不支持vista系统
        StrCpy $R1 "$QUICKLAUNCH\User Pinned\TaskBar"
        ${StdUtils.InvokeShellVerb} $0 "$R1" "${PRODUCT_NAME}.lnk" ${StdUtils.Const.ShellVerb.UnpinFromTaskbar}
    ${Else}
    	Delete "$QUICKLAUNCH\${PRODUCT_NAME}.lnk"
  	${EndIf}

	Delete "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk"
	Delete "$SMPROGRAMS\${PRODUCT_NAME}\卸载${PRODUCT_NAME}.lnk"
  	Delete "$DESKTOP\${PRODUCT_NAME}.lnk" ; 桌面图标
  	Delete "$SMSTARTUP\${PRODUCT_NAME}.lnk" ; 开机启动项
  	RMDir /r /REBOOTOK "$SMPROGRAMS\${PRODUCT_NAME}"

  	${un.RefreshShellIcons}

  	SetShellVarContext current
  	RMDir /r /REBOOTOK "$SMPROGRAMS\${PRODUCT_NAME}"
FunctionEnd

Function un.RemoveFiles
	${Locate} "$INSTDIR" "/M=*.*" "un.OnDeleteFileFound"
	StrCpy $InstallState "done"
	${nsdm.SetControlAttr} "btn_close" 0 "bdisable" 
	${nsdm.SetControlAttr} "progress_uninstalling" 100 "value"
FunctionEnd

Function un.OnDeleteFileFound
	; $R9    "path\name"
	; $R8    "path"
	; $R7    "name"
	; $R6    "size"  ($R6="" if directory, $R6="0" if file with /S=)

	; $R0-$R5  are not used (save data in them).
	; ...

	; Push $var    ; If $var="StopLocate" Then exit from function
	Delete "$R9"
	RMDir /r /REBOOTOK "$R9"
	IntOp $UninstallProgress $UninstallProgress + 2
	${If} $UninstallProgress > 100
		IntOp $UninstallProgress 100 + 0
	${EndIf}
	${nsdm.SetControlAttr} "progress_uninstalling" $UninstallProgress "value"

	Push "LocateNext"
FunctionEnd

Function un.DMPage
	StrCpy $InstallState "doing"

	!insertmacro NSDM_DEBUG_INFO "初始化DM库"
	${nsdm.InitSkinEngine} ${DM_SKIN_PATH} "dui_uninstall_wnd" "uninstall_frame" ${NSDM_OUT}

	!insertmacro NSDM_DEBUG_INFO "${NSDM_OUT}"
	Call un.BindUnInstUIControls
	Call un.InitDialog

	!insertmacro PostPasEvent ${PAS_BEGIN_UNINSTALL}

	${nsdm.Run}
	!insertmacro NSDM_DEBUG_INFO "卸载DM库"

	!insertmacro PostSessionData
	${nscurl.WaitDone} 5000 $0
FunctionEnd

Function un.InitDialog
	${nsdm.SetControlAttr} "label_install_title" "${PRODUCT_DESCRIPTION}${PRODUCT_VERSION}" "text"
FunctionEnd

Function un.onInit
	; 检查卸载进程是否在运行
	!insertmacro "CheckSetupRunning" ${SETUP_MUTEX_NAME} ${NSDM_OUT}
	${If} ${NSDM_OUT} == 1
		MessageBox MB_OK|MB_ICONEXCLAMATION "安装或卸载程序已经在运行。"
   		Abort
	${EndIf}

	!insertmacro "NSDM_DEBUG_INFO" "un.onInit"

	InitPluginsDir
	File "/ONAME=${DM_SKIN_PATH}" "..\skin\skin.zip"

	!insertmacro SetSessionId
	!insertmacro PostSessionData
FunctionEnd

Function un.BindUnInstUIControls
	; 全局
	GetFunctionAddress $0 "un.OnExitDUISetup"
	${nsdm.RegisterCommandNotify} "btn_close" $0

	; 卸载过程窗口
	GetFunctionAddress $0 "un.OnExitDUISetup"
	${nsdm.RegisterCommandNotify} "btn_uninstall_cancel" $0

	GetFunctionAddress $0 "un.UninstallFunc"
	${nsdm.RegisterCommandNotify} "btn_uninstall_ok" $0

	; 卸载完成窗口
	GetFunctionAddress $0 "un.OnBtnCommit"
	${nsdm.RegisterCommandNotify} "btn_commit" $0

	GetFunctionAddress $0 "un.OnBtnDownload"
	${nsdm.RegisterCommandNotify} "btn_download_client" $0
FunctionEnd

Function un.OnBtnCommit
	; 隐藏主窗口
	${nsdm.SetWindowAttr} "0" "bvisible"

	; 上报卸载反馈
	Call un.BuildUserFeedback

	; 退出
	Call un.OnExitDUISetup
FunctionEnd

Function un.BuildUserFeedback
	; 获取用户反馈信息
	${nsdm.GetControlAttr} "btn_reason_reinstall" "check" $1
	${nsdm.GetControlAttr} "btn_reason_no_love" "check" $2
	${nsdm.GetControlAttr} "btn_reason_no_game" "check" $3
	${nsdm.GetControlAttr} "btn_reason_performance" "check" $4
	${nsdm.GetControlAttr} "edit_reason" "text" $5
	!insertmacro "NSDM_DEBUG_INFO" "卸载原因： $5"
	nsutil::NSUrlEncode "$5"
	Pop $5

	StrCpy $R0 "http://lobbydata.game.yy.com/uninstall/add.do?product=${PRODUCT_CHANNEL_NAME}&reasonType="
	${If} $1 == 1
		StrCpy $R0 "$R01|"
	${EndIf}
	${If} $2 == 1
		StrCpy $R0 "$R02|"
	${EndIf}
	${If} $3 == 1
		StrCpy $R0 "$R03|"
	${EndIf}
	${If} $4 == 1
		StrCpy $R0 "$R04"
	${EndIf}
	StrCpy $R0 "$R0&reasonContext=$5"

	${nscurl.Get} $R0 ${NSDM_OUT}
	!insertmacro "NSDM_DEBUG_INFO" "${NSDM_OUT}"
FunctionEnd

Function un.OnBtnDownload
	${nsdm.SendMessage} "UM_NSDM_OPEN_URL" "${PRODUCT_WEB_SITE}" ""
	!insertmacro PostPasEvent ${PAS_DOWNLOAD_CLIENT}
FunctionEnd

Function un.OnExitDUISetup
	!insertmacro "NSDM_DEBUG_INFO" "$InstallState"
	${If} $InstallState == "doing"
		!insertmacro PostPasEvent ${PAS_UNINSTALL_CANCELED}
	${EndIf}

	!insertmacro PostPasEvent ${PAS_END_UNINSTALL}
	${nsdm.UninitSkinEngine} 
FunctionEnd

Function un.TerminateIfExists
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

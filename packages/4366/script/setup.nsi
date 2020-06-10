Unicode true

# ====================== 安装程序初始定义常量 ==============================
# !define _DEBUG 1

SetCompressor lzma
# !packhdr "$%TEMP%\yygame_exehead.tmp" '"..\..\..\script\upx.exe" -9 "$%TEMP%\yygame_exehead.tmp"'

# ====================== product ==============================
!include "product.nsh"

# ===================== 全局设置 =============================
!ifndef _DEBUG
	!verbose 3 ; 加快编译速度
!endif

# ===================== MUI设置 =============================
!include "MUI.nsh"

# ===================== 文件打包 =============================
ReserveFile "..\skin\skin.zip"
# ReserveFile "..\addon\Launcher.exe"
; ReserveFile "..\addon\license.txt"
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

# =====================  =============================
!addincludedir "..\..\..\script"
!include "utils.nsh"
!include "nsdm.nsh"
!include "WinVer.nsh"
!include "StdUtils.nsh"
!include "nsCurl.nsh"
!include "WordFunc.nsh"
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

# ==================== 自定义页面 ================================
Page custom DMPage

UninstPage custom un.DMPage

# ==================== 全局变量 ================================
!define DM_SKIN_PATH "$PLUGINSDIR\skin.zip"
!define GAME_LICENSE_PATH "$PLUGINSDIR\license.txt"
!define VC_REDIST_PATH "$PLUGINSDIR\vcredist_x86.exe"
!define MB_OKCANCEL 1
!define MB_RETRYCANCEL 5
!define IDOK 1
!define IDCANCEL 2
!define IDRETRY 4
!define IDCLOSE 8
Var InstallState ; 安装状态(doing、done)
Var CountdownTimer ; 安装完成界面倒计时定时器
Var Countdown ; 倒计时计数
Var InstallTimer ; 开始安装定时器，因为安装速度太快，所以放在定时器，进行延时安装
Var CheckClientCount ; 检测客户端是否在运行的次数
Var UninstallProgress ; 卸载进度
Var InstallRoot ; 安装根目录
Var PasSessionId ; pas上报session id
Var MBSize ; 弹出框大小
Var OverwriteInstall ; 是否覆盖安装: 0 未检测到已安装版本; 1 覆盖安装; 2 取消安装
Var NSDM_OUT

Section "-Dummy"
	; 空白区段，防止编译器报错
	Nop
SectionEnd
# ==================== 安装区段 ================================
Function InstallFunc
	;复制要发布的安装文件
	SetOutPath "$INSTDIR"
	SetOverWrite on
	# 先释放Launcher程序
	; File "..\addon\Launcher.exe"
	; File /r /x .svn "..\product\*.*"
	SetCompress off
	File "x-studio-Pro_10.0.9000.7.exe"
	SetCompress auto
	SetRebootFlag false

	# GetFunctionAddress $R9 ExtractCallback
	# Nsis7z::ExtractWithCallback "$OUTDIR\product.7z" $R9

	# 文件解压有可能失败，先判断目标文件是否存在
	IfFileExists "$INSTDIR\${PRODUCT_VERSION}\${PRODUCT_MAIN_EXE}" done 0
	!insertmacro "NSDM_DEBUG_INFO" "第一次释放文件失败，尝试进行第二次释放"
	# 这里不跳转回去，因为循环跳转要加次数判断，太麻烦
	# GetFunctionAddress $R9 ExtractCallback
	# Nsis7z::ExtractWithCallback "$OUTDIR\product.7z" $R9

	IfFileExists "$INSTDIR\${PRODUCT_VERSION}\${PRODUCT_MAIN_EXE}" done 0
	!insertmacro "NSDM_DEBUG_INFO" "第二次释放文件失败"
	Delete "$OUTDIR\product.7z"
	Return
	done:
	!insertmacro "NSDM_DEBUG_INFO" "释放文件成功"
	# Delete "$OUTDIR\product.7z"
	# ini文件根据需求动态生成
	# File "..\addon\Version.ini"
FunctionEnd

Function ExtractCallback
	Pop $R8 ; 已安装大小
  	Pop $R9 ; 总大小
  	System::Int64Op $R8 * 100
  	Pop $R7
  	System::Int64Op $R7 / $R9
  	Pop $R7 ; 安装百分比
  	${nsdm.SetControlAttr} "progress_installing" $R7 "value"
  	${nsdm.SetControlAttr} "label_install_percent" "$R7%" "text"

  	${If} $R8 == $R9
        ${nsdm.SetControlAttr} "progress_installing" 100 "value"
		${nsdm.SetControlAttr} "label_install_percent" "100%" "text"
    ${EndIf}
FunctionEnd

; 生成快捷方式
Function BuildShortCut
	SetShellVarContext ${PRODUCT_LNK_USER}
  	CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" '${PRODUCT_DESKTOP_ARG}'

  	Call CreateTaskbar
  	Call CreateStartMenu

  	${RefreshShellIcons}
  	
  	SetShellVarContext current
FunctionEnd

; 创建开始菜单快捷方式
Function CreateStartMenu
	CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" '${PRODUCT_STARTMENU_ARG}'
	# CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\卸载${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_UNINSTALL_NAME}"

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
	# File "..\addon\${PRODUCT_UNINSTALL_NAME}"
	
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

; DM页面
Function DMPage
	StrCpy $InstallState "doing"

	!insertmacro NSDM_DEBUG_INFO "初始化DM库"
	${nsdm.InitSkinEngine} ${DM_SKIN_PATH} "dui_install_wnd" "install_frame" $NSDM_OUT

	Call BindUIControls

	Call InitDialog

	Call CheckInstalled

	${If} $OverwriteInstall == 2
		!insertmacro NSDM_DEBUG_INFO "安装包版本号不大于本地版本号，直接启动客户端"
		Call OnBtnStartGame
	${Else}
		!insertmacro NSDM_DEBUG_INFO "启动安装定时器，准备进行安装"
		StrCpy $CheckClientCount 0
		GetFunctionAddress $InstallTimer OnInstallTimer
		${nsdm.CreateTimer} $InstallTimer 800
		${nsdm.Run}
	${EndIf}
	!insertmacro NSDM_DEBUG_INFO "卸载DM库"

	!insertmacro PostSessionData
	${nscurl.WaitDone} 10000 $0
FunctionEnd

Function InitDialog
	StrCpy $NSDM_OUT $INSTDIR
	Call SetInstallFolder

	StrCpy $MBSize "586,436"

	StrCpy $Countdown 0
	GetFunctionAddress $CountdownTimer OnShufflingFigureTimer
	${nsdm.CreateTimer} $CountdownTimer 1000

	!insertmacro PostPasEvent ${PAS_BEGIN_INSTALL} 
FunctionEnd

; 检测是否已经安装客户端
Function CheckInstalled
	StrCpy $1 "$INSTDIR\${PRODUCT_MAIN_EXE}"
	StrCpy $2 "$INSTDIR\Version.ini"
	StrCpy $OverwriteInstall 0
	IfFileExists "$1" 0 out
	
	ReadINIStr $3 "$2" "4366game" "NewVersion"
	${If} "$3" == ""
		# 用户没有安装客户端
		StrCpy $OverwriteInstall 0
	${Else}
		${VersionCompare} "${PRODUCT_VERSION}" "$3" $4
		${If} $4 == 1
			# 安装包版本较新
			StrCpy $OverwriteInstall 1
		${Else}
			# 本地版本较新
			# 或者版本相同
			StrCpy $OverwriteInstall 2
		${EndIf}
		!insertmacro "NSDM_DEBUG_INFO" "${PRODUCT_VERSION}与$3的比较结果为$4"
	${EndIf}

	# 不存在launcher则直接返回，无视配置文件
	out:
FunctionEnd

Function .onInit
	; 检查安装进程是否在运行
	!insertmacro "CheckSetupRunning" ${SETUP_MUTEX_NAME} $NSDM_OUT
	${If} $NSDM_OUT == 1
		# MessageBox MB_OK|MB_ICONEXCLAMATION "安装或卸载程序已经在运行。"
   		Abort
	${EndIf}
	
	InitPluginsDir
	File "/ONAME=${DM_SKIN_PATH}" "..\skin\skin.zip"
	; File "/ONAME=${GAME_LICENSE_PATH}" "..\addon\license.txt"
	; File "/ONAME=${VC_REDIST_PATH}" "..\addon\vcredist_x86.exe"

	!insertmacro SetSessionId
	!insertmacro PostSessionData
FunctionEnd

; 安装界面点击退出，给出提示
Function OnExitDUISetup
	!insertmacro "NSDM_DEBUG_INFO" "即将退出，当前安装状态为：$InstallState"
	${If} $InstallState == "doing"
		!insertmacro PostPasEvent ${PAS_INSTALL_CANCELED}
	${EndIf}
	${nsdm.KillTimer} $CountdownTimer
	${nsdm.UninitSkinEngine}
	!insertmacro PostPasEvent ${PAS_END_INSTALL}
FunctionEnd

Function OnBtnClickLicense
	${nsdm.ShowLicense} "dui_license_wnd" "label_install_title" "${PRODUCT_DESCRIPTION}" "edit_install_license" ${GAME_LICENSE_PATH} $NSDM_OUT
FunctionEnd

Function OnBtnSelectPath
	${nsdm.SelectFolderDialog} "${PRODUCT_DESCRIPTION}" $NSDM_OUT
	!insertmacro NSDM_DEBUG_INFO "$NSDM_OUT"
	Call SetInstallFolder
FunctionEnd

Function OnBtnStartGame
	!insertmacro "NSDM_DEBUG_INFO" "OnBtnStartGame-->$INSTDIR\${PRODUCT_MAIN_EXE} ${PRODUCT_DESKTOP_ARG}"
	Exec '"$INSTDIR\${PRODUCT_MAIN_EXE}" ${PRODUCT_DESKTOP_ARG}'
	!insertmacro PostPasEvent ${PAS_RUN_CLIENT}
	Call OnExitDUISetup
FunctionEnd

Function SetInstallFolder
	!insertmacro "NSDM_DEBUG_INFO" "SetInstallFolder: $NSDM_OUT"
	${If} "$NSDM_OUT" != "0"
	${AndIf} "$NSDM_OUT" != ""
		;${If} $OverwriteInstall == 0
		;	; 非覆盖安装模式下，才允许调整安装路径
		;	nsutil::NSCreateSeparatePath "$NSDM_OUT" "${PRODUCT_NAME}"
		;	Pop $NSDM_OUT
		;${EndIf}
		StrCpy $INSTDIR "$NSDM_OUT"
		${GetRoot} "$NSDM_OUT" $InstallRoot

		# ${nsdm.SetControlAttr} "edit_install_folder" "$NSDM_OUT" "text"

		# !insertmacro "FormatSizeString" ${PRODUCT_INSTALL_SIZE} $R0
		# StrCpy $R1 "所需空间: $R0"
		# ${nsdm.SetControlAttr} "label_install_size" $R1 "text"
		
		# !insertmacro "GetFreeSpaceSize" "$NSDM_OUT" $R0
		# !insertmacro "FormatSizeString" $R0 $R0
		# StrCpy $R1 "磁盘可用空间: $R0"
		# ${nsdm.SetControlAttr} "label_useable_size" $R1 "text"

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
	# ${nsdm.GetControlAttr} "edit_install_folder" "text" $NSDM_OUT
	# Call SetInstallFolder
	
	;生成最终的安装路径
	# ${If} $OverwriteInstall == 0
	# 	; 非覆盖安装模式下，才设置最终路径
	# 	Call SetLastInstallPath
	# ${EndIf}
	
	
	; 检查磁盘空间是否足够
	!insertmacro "GetFreeSpaceSize" "$NSDM_OUT" $R0
	System::Int64Op $R0 > ${PRODUCT_INSTALL_SIZE}
	Pop $0
	${If} $0 == 0
		# ${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "错误提示" "label_popup_content" "磁盘空间不足，请选择其他磁盘!" ${MB_OKCANCEL} $0
        !insertmacro "NSDM_DEBUG_INFO" "$NSDM_OUT可用空间$R0，客户端需要空间${PRODUCT_INSTALL_SIZE}"
		!insertmacro PostPasEvent ${PAS_INSTALL_FAILED_DESKSPACE}
		MessageBox MB_OK|MB_ICONEXCLAMATION "检测到系统盘空间不足，可能会影响大厅正常运行，建议清理系统盘空间。"
		Call OnBtnStartGame
		Return
	${EndIf}

	; 判断是否覆盖安装
	${If} $OverwriteInstall == 0
		#  全新安装
		!insertmacro PostPasEvent ${PAS_INSTALL_DEFAULT}
	${ElseIf} $OverwriteInstall == 1
		#  覆盖安装
		!insertmacro PostPasEvent ${PAS_INSTALL_CUSTOM}
	${EndIf}

	SetRegView 32
	
	; 启动一个低优先级的后台线程
    GetFunctionAddress $0 InstallFunc
    # BgWorker::CallAndWait

	IfFileExists "$INSTDIR\${PRODUCT_VERSION}\${PRODUCT_MAIN_EXE}" unpack_success 0
	!insertmacro "NSDM_DEBUG_INFO" "文件解压失败，退出安装"
	!insertmacro PostPasEvent ${PAS_INSTALL_FAILED_UNPACK}
	Call OnBtnStartGame
	Return

	unpack_success:
	Call BuildShortCut
	# Call BuildUninstaller

	StrCpy $InstallState "done"
	!insertmacro PostPasEvent ${PAS_INSTALL_SUCCESS}
	!insertmacro PostPasEvent ${PAS_INSTALL_CHANNEL}
	
	ReadINIStr $1 "$INSTDIR\Version.ini" "4366game" "NewVersion"
	${If} "$1" == ""
		WriteINIStr "$INSTDIR\Version.ini" "4366game" "NewVersion" "${PRODUCT_VERSION}"
		WriteINIStr "$INSTDIR\Version.ini" "4366game" "OldVersion" "${PRODUCT_VERSION}"
		WriteINIStr "$INSTDIR\Version.ini" "4366game" "Version" "${PRODUCT_VERSION}"
	${Else}
		WriteINIStr "$INSTDIR\Version.ini" "4366game" "NewVersion" "${PRODUCT_VERSION}"
		WriteINIStr "$INSTDIR\Version.ini" "4366game" "Version" "${PRODUCT_VERSION}"
		WriteINIStr "$INSTDIR\Version.ini" "4366game" "OldVersion" "$1"
	${EndIf}

	WriteINIStr "$INSTDIR\Config.ini" "pas" "channel" "${PRODUCT_CHANNEL_NAME}"
	WriteINIStr "$INSTDIR\Config.ini" "pas" "channeltag" "${PRODUCT_CHANNEL_TAG}"
	
	Call OnBtnStartGame
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
	${nsdm.GetControlAttr} "edit_install_folder" "text" $NSDM_OUT
	Push $0
	${GetRoot} "$NSDM_OUT" $0
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

; Function OnCountdownTimer
; 	!insertmacro "NSDM_DEBUG_INFO" "启动游戏倒计时: $Countdown"

; 	IntOp $Countdown $Countdown - 1
; 	${If} $Countdown == 0
; 		${nsdm.KillTimer} $CountdownTimer
; 		Call OnBtnStartGame
; 	${Else}
; 		StrCpy $0 "$Countdown s后自动体验游戏大厅"
; 		${nsdm.SetControlAttr} "label_counter_start" "$0" "text"
; 	${EndIf}
; FunctionEnd

Function OnShufflingFigureTimer 
	!insertmacro "NSDM_DEBUG_INFO" "轮播图定时器 $Countdown"
	IntOp $Countdown $Countdown + 1
	${If} $Countdown > 3
		StrCpy $Countdown 1
	${EndIf}
	${nsdm.SetControlAttr} "wnd_shuffling_figure" "shuffling_figure$Countdown" "skin"
FunctionEnd

Function OnInstallTimer
	; 检查客户端进程是否在运行
	!insertmacro "ProcessDetect" "${PRODUCT_CLIENT_EXE}" ";" "$INSTDIR"
	Pop $0
	${If} $0 == 0
	    # 目标进程没有运行
		${nsdm.KillTimer} $InstallTimer
		!insertmacro "NSDM_DEBUG_INFO" "进程没有在运行，直接进入安装流程"
		Call OnBtnClickInstall
	${Else}
		IntOp $CheckClientCount $CheckClientCount + 1
		!insertmacro "NSDM_DEBUG_INFO" "进程正在运行，等待结束$CheckClientCount"
		${If} $CheckClientCount >= 3
			#  等待超时
			${nsdm.KillTimer} $InstallTimer
			!insertmacro "NSDM_DEBUG_INFO" "等待指定进程结束超时"
			!insertmacro PostPasEvent ${PAS_INSTALL_FAILED_TIMEOUT}
			Call OnBtnStartGame
		${EndIf}
	${EndIf}
FunctionEnd

Function TerminateIfExists
	Pop $R2 ; 进程名集合

	!insertmacro "ProcessDetect" "$R2" ";" ""
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
	# BgWorker::CallAndWait

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
	${nsdm.InitSkinEngine} ${DM_SKIN_PATH} "dui_uninstall_wnd" "uninstall_frame" $NSDM_OUT

	!insertmacro NSDM_DEBUG_INFO "$NSDM_OUT"
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
	!insertmacro "CheckSetupRunning" ${SETUP_MUTEX_NAME} $NSDM_OUT
	${If} $NSDM_OUT == 1
		MessageBox MB_OK|MB_ICONEXCLAMATION "安装或卸载程序已经在运行。"
   		Abort
	${EndIf}

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

	${nscurl.Get} $R0 $NSDM_OUT
	!insertmacro "NSDM_DEBUG_INFO" "$NSDM_OUT"
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

	!insertmacro "ProcessDetect" "$R2" ";" ""
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

# ====================== ��װ�����ʼ���峣�� ==============================
# !define _DEBUG 1
SetCompressor lzma
!packhdr "$%TEMP%\yygame_exehead.tmp" '"..\..\..\script\upx.exe" -9 "$%TEMP%\yygame_exehead.tmp"'

# ====================== ��Ʒ�Զ�����Ϣ ==============================
!include "product.nsh"

# ===================== ȫ������ =============================
!ifndef _DEBUG
	!verbose 3 ; �ӿ�����ٶ�
!endif

# ===================== MUI���� =============================
!include "MUI.nsh"

# ===================== �ļ���� =============================
ReserveFile "..\skin\skin.zip"
# ReserveFile "..\addon\Launcher.exe"
; ReserveFile "..\addon\license.txt"
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS ; ��װԤ�ͷ��ļ�

# ===================== ���������ͷ�ļ��Ͳ�� =============================
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

# ==================== NSIS���� ================================
BrandingText /TRIMLEFT "${PRODUCT_PUBLISHER}"
SetFont "΢���ź�" 8
CRCCheck on
InstallDirRegKey ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "installDir" ; ��ע����ȡ��װ·��
RequestExecutionLevel admin ; Ȩ������ none|user|highest|admin
Icon "..\skin\install.ico" ; ��װͼ��
UninstallIcon "..\skin\uninstall.ico" ; ж��ͼ��
ShowInstDetails nevershow ; �����Ƿ���ʾ��װ��ϸ��Ϣ
ShowUninstDetails nevershow ; �����Ƿ���ʾж����ϸ��Ϣ
!insertmacro MUI_LANGUAGE "SimpChinese"
;SilentUnInstall silent ; ��Ĭ��װ
;SilentInstall silent ; ��Ĭж��

# ===================== ��װ���汾 =============================
VIProductVersion                    "${PRODUCT_VERSION}"
VIAddVersionKey "ProductVersion"    "${PRODUCT_VERSION}"
VIAddVersionKey "ProductName"       "${PRODUCT_NAME}"
VIAddVersionKey "CompanyName"       "${PRODUCT_PUBLISHER}"
VIAddVersionKey "FileVersion"       "${PRODUCT_VERSION}"
VIAddVersionKey "InternalName"      "${PRODUCT_NAME}.exe"
VIAddVersionKey "FileDescription"   "${PRODUCT_DESCRIPTION}"
VIAddVersionKey "LegalCopyright"    "${PRODUCT_LEGAL}"

# ==================== �Զ���ҳ�� ================================
Page custom DMPage

UninstPage custom un.DMPage

# ==================== ȫ�ֱ��� ================================
!define DM_SKIN_PATH "$PLUGINSDIR\skin.zip"
!define GAME_LICENSE_PATH "$PLUGINSDIR\license.txt"
!define VC_REDIST_PATH "$PLUGINSDIR\vcredist_x86.exe"
!define MB_OKCANCEL 1
!define MB_RETRYCANCEL 5
!define IDOK 1
!define IDCANCEL 2
!define IDRETRY 4
!define IDCLOSE 8
Var InstallState ; ��װ״̬(doing��done)
Var CountdownTimer ; ��װ��ɽ��浹��ʱ��ʱ��
Var Countdown ; ����ʱ����
Var InstallTimer ; ��ʼ��װ��ʱ������Ϊ��װ�ٶ�̫�죬���Է��ڶ�ʱ����������ʱ��װ
Var CheckClientCount ; ���ͻ����Ƿ������еĴ���
Var UninstallProgress ; ж�ؽ���
Var InstallRoot ; ��װ��Ŀ¼
Var PasSessionId ; pas�ϱ�session id
Var MBSize ; �������С
Var OverwriteInstall ; �Ƿ񸲸ǰ�װ: 0 δ��⵽�Ѱ�װ�汾; 1 ���ǰ�װ; 2 ȡ����װ
Var NSDM_OUT

Section "-Dummy"
	; �հ����Σ���ֹ����������
	Nop
SectionEnd
# ==================== ��װ���� ================================
Function InstallFunc
	;����Ҫ�����İ�װ�ļ�
	SetOutPath "$INSTDIR"
	SetOverWrite on
	# ���ͷ�Launcher����
	File "..\addon\Launcher.exe"
	; File /r /x .svn "..\product\*.*"
	SetCompress off
	File "..\product\product.7z"
	SetCompress auto
	SetRebootFlag false

	GetFunctionAddress $R9 ExtractCallback
	Nsis7z::ExtractWithCallback "$OUTDIR\product.7z" $R9

	# �ļ���ѹ�п���ʧ�ܣ����ж�Ŀ���ļ��Ƿ����
	IfFileExists "$INSTDIR\${PRODUCT_VERSION}\${PRODUCT_MAIN_EXE}" done 0
	!insertmacro "NSDM_DEBUG_INFO" "��һ���ͷ��ļ�ʧ�ܣ����Խ��еڶ����ͷ�"
	# ���ﲻ��ת��ȥ����Ϊѭ����תҪ�Ӵ����жϣ�̫�鷳
	GetFunctionAddress $R9 ExtractCallback
	Nsis7z::ExtractWithCallback "$OUTDIR\product.7z" $R9

	IfFileExists "$INSTDIR\${PRODUCT_VERSION}\${PRODUCT_MAIN_EXE}" done 0
	!insertmacro "NSDM_DEBUG_INFO" "�ڶ����ͷ��ļ�ʧ��"
	Delete "$OUTDIR\product.7z"
	Return
	done:
	!insertmacro "NSDM_DEBUG_INFO" "�ͷ��ļ��ɹ�"
	Delete "$OUTDIR\product.7z"
	# ini�ļ���������̬����
	# File "..\addon\Version.ini"
FunctionEnd

Function ExtractCallback
	Pop $R8 ; �Ѱ�װ��С
  	Pop $R9 ; �ܴ�С
  	System::Int64Op $R8 * 100
  	Pop $R7
  	System::Int64Op $R7 / $R9
  	Pop $R7 ; ��װ�ٷֱ�
  	${nsdm.SetControlAttr} "progress_installing" $R7 "value"
  	${nsdm.SetControlAttr} "label_install_percent" "$R7%" "text"

  	${If} $R8 == $R9
        ${nsdm.SetControlAttr} "progress_installing" 100 "value"
		${nsdm.SetControlAttr} "label_install_percent" "100%" "text"
    ${EndIf}
FunctionEnd

; ���ɿ�ݷ�ʽ
Function BuildShortCut
	SetShellVarContext ${PRODUCT_LNK_USER}
  	CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" '${PRODUCT_DESKTOP_ARG}'

  	Call CreateTaskbar
  	Call CreateStartMenu

  	${RefreshShellIcons}
  	
  	SetShellVarContext current
FunctionEnd

; ������ʼ�˵���ݷ�ʽ
Function CreateStartMenu
	CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" '${PRODUCT_STARTMENU_ARG}'
	# CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\ж��${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_UNINSTALL_NAME}"

	!insertmacro PostPasEvent ${PAS_INSTALL_WITH_STARTMENU}
FunctionEnd

; ����������ͼ��
Function CreateTaskbar
    ${If} ${IsWin7}
    ${OrIf} ${IsWinVista}
    	; ɾ������ͼ��
    	StrCpy $R1 "$QUICKLAUNCH\User Pinned\TaskBar\${PRODUCT_NAME}.lnk"
  		ExecShell "taskbarunpin" "$R1"
  		Delete "$R1"
  		; ������ͼ��
    	StrCpy $R1 "$INSTDIR\${PRODUCT_NAME}.lnk"
  		CreateShortCut "$R1" "$INSTDIR\${PRODUCT_MAIN_EXE}" '${PRODUCT_TASKBAR_ARG}'
  		ExecShell "taskbarpin" "$R1"
  		Delete "$R1"
    ${ElseIf} ${AtLeastWin7}
    	; ɾ������ͼ��
    	StrCpy $R1 "$QUICKLAUNCH\User Pinned\TaskBar"
        ${StdUtils.InvokeShellVerb} $0 "$R1" "${PRODUCT_NAME}.lnk" ${StdUtils.Const.ShellVerb.UnpinFromTaskbar}

    	; ʹ�ò������������ͼ��
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

; ����ж�����
Function BuildUninstaller
	;WriteUninstaller "$INSTDIR\${PRODUCT_UNINSTALL_NAME}"   ;Create uninstaller
	# File "..\addon\${PRODUCT_UNINSTALL_NAME}"
	
	; ���ж����Ϣ���������
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

; DMҳ��
Function DMPage
	StrCpy $InstallState "doing"

	!insertmacro NSDM_DEBUG_INFO "��ʼ��DM��"
	${nsdm.InitSkinEngine} ${DM_SKIN_PATH} "dui_install_wnd" "install_frame" $NSDM_OUT

	Call BindUIControls

	Call InitDialog

	Call CheckInstalled

	${If} $OverwriteInstall == 2
		!insertmacro NSDM_DEBUG_INFO "��װ���汾�Ų����ڱ��ذ汾�ţ�ֱ�������ͻ���"
		Call OnBtnStartGame
	${Else}
		!insertmacro NSDM_DEBUG_INFO "������װ��ʱ����׼�����а�װ"
		StrCpy $CheckClientCount 0
		GetFunctionAddress $InstallTimer OnInstallTimer
		${nsdm.CreateTimer} $InstallTimer 800
		${nsdm.Run}
	${EndIf}
	!insertmacro NSDM_DEBUG_INFO "ж��DM��"

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

; ����Ƿ��Ѿ���װ�ͻ���
Function CheckInstalled
	StrCpy $1 "$INSTDIR\${PRODUCT_MAIN_EXE}"
	StrCpy $2 "$INSTDIR\Version.ini"
	StrCpy $OverwriteInstall 0
	IfFileExists "$1" 0 out
	
	ReadINIStr $3 "$2" "4366game" "NewVersion"
	${If} "$3" == ""
		# �û�û�а�װ�ͻ���
		StrCpy $OverwriteInstall 0
	${Else}
		${VersionCompare} "${PRODUCT_VERSION}" "$3" $4
		${If} $4 == 1
			# ��װ���汾����
			StrCpy $OverwriteInstall 1
		${Else}
			# ���ذ汾����
			# ���߰汾��ͬ
			StrCpy $OverwriteInstall 2
		${EndIf}
		!insertmacro "NSDM_DEBUG_INFO" "${PRODUCT_VERSION}��$3�ıȽϽ��Ϊ$4"
	${EndIf}

	# ������launcher��ֱ�ӷ��أ����������ļ�
	out:
FunctionEnd

Function .onInit
	; ��鰲װ�����Ƿ�������
	!insertmacro "CheckSetupRunning" ${SETUP_MUTEX_NAME} $NSDM_OUT
	${If} $NSDM_OUT == 1
		# MessageBox MB_OK|MB_ICONEXCLAMATION "��װ��ж�س����Ѿ������С�"
   		Abort
	${EndIf}
	
	InitPluginsDir
	File "/ONAME=${DM_SKIN_PATH}" "..\skin\skin.zip"
	; File "/ONAME=${GAME_LICENSE_PATH}" "..\addon\license.txt"
	; File "/ONAME=${VC_REDIST_PATH}" "..\addon\vcredist_x86.exe"

	!insertmacro SetSessionId
	!insertmacro PostSessionData
FunctionEnd

; ��װ�������˳���������ʾ
Function OnExitDUISetup
	!insertmacro "NSDM_DEBUG_INFO" "�����˳�����ǰ��װ״̬Ϊ��$InstallState"
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
		;	; �Ǹ��ǰ�װģʽ�£������������װ·��
		;	nsutil::NSCreateSeparatePath "$NSDM_OUT" "${PRODUCT_NAME}"
		;	Pop $NSDM_OUT
		;${EndIf}
		StrCpy $INSTDIR "$NSDM_OUT"
		${GetRoot} "$NSDM_OUT" $InstallRoot

		# ${nsdm.SetControlAttr} "edit_install_folder" "$NSDM_OUT" "text"

		# !insertmacro "FormatSizeString" ${PRODUCT_INSTALL_SIZE} $R0
		# StrCpy $R1 "����ռ�: $R0"
		# ${nsdm.SetControlAttr} "label_install_size" $R1 "text"
		
		# !insertmacro "GetFreeSpaceSize" "$NSDM_OUT" $R0
		# !insertmacro "FormatSizeString" $R0 $R0
		# StrCpy $R1 "���̿��ÿռ�: $R0"
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
	${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "������ʾ" "label_popup_content" "��ǰ�����޷���װ${PRODUCT_NAME}����װ���򼴽��˳���" ${MB_OKCANCEL} $0
	StrCpy $InstallState "done"
	Call OnExitDUISetup
FunctionEnd

#�������հ�װ·��
Function SetLastInstallPath
	StrCpy $INSTDIR "$INSTDIR\${PRODUCT_NAME}"
FunctionEnd

Function OnBtnClickInstall
	; ȷ����װ·��Ϊ�û�����ѡ��·��
	# ${nsdm.GetControlAttr} "edit_install_folder" "text" $NSDM_OUT
	# Call SetInstallFolder
	
	;�������յİ�װ·��
	# ${If} $OverwriteInstall == 0
	# 	; �Ǹ��ǰ�װģʽ�£�����������·��
	# 	Call SetLastInstallPath
	# ${EndIf}
	
	
	; �����̿ռ��Ƿ��㹻
	!insertmacro "GetFreeSpaceSize" "$NSDM_OUT" $R0
	System::Int64Op $R0 > ${PRODUCT_INSTALL_SIZE}
	Pop $0
	${If} $0 == 0
		# ${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "������ʾ" "label_popup_content" "���̿ռ䲻�㣬��ѡ����������!" ${MB_OKCANCEL} $0
        !insertmacro "NSDM_DEBUG_INFO" "$NSDM_OUT���ÿռ�$R0���ͻ�����Ҫ�ռ�${PRODUCT_INSTALL_SIZE}"
		!insertmacro PostPasEvent ${PAS_INSTALL_FAILED_DESKSPACE}
		MessageBox MB_OK|MB_ICONEXCLAMATION "��⵽ϵͳ�̿ռ䲻�㣬���ܻ�Ӱ������������У���������ϵͳ�̿ռ䡣"
		Call OnBtnStartGame
		Return
	${EndIf}

	; �ж��Ƿ񸲸ǰ�װ
	${If} $OverwriteInstall == 0
		#  ȫ�°�װ
		!insertmacro PostPasEvent ${PAS_INSTALL_DEFAULT}
	${ElseIf} $OverwriteInstall == 1
		#  ���ǰ�װ
		!insertmacro PostPasEvent ${PAS_INSTALL_CUSTOM}
	${EndIf}

	SetRegView 32
	
	; ����һ�������ȼ��ĺ�̨�߳�
    GetFunctionAddress $0 InstallFunc
    BgWorker::CallAndWait

	IfFileExists "$INSTDIR\${PRODUCT_VERSION}\${PRODUCT_MAIN_EXE}" unpack_success 0
	!insertmacro "NSDM_DEBUG_INFO" "�ļ���ѹʧ�ܣ��˳���װ"
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

; �Զ��尲װcheckbox
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

; Э��checkbox
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

; ��edit�ؼ����ݸı�ʱ����
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

; �󶨰�װ�����¼�
Function BindUIControls
	; ȫ��
	GetFunctionAddress $0 "OnExitDUISetup"
	${nsdm.RegisterCommandNotify} "btn_close" $0

	; ��ҳ
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

	; ��װ���ҳ��
	GetFunctionAddress $0 "OnBtnStartGame"
	${nsdm.RegisterCommandNotify} "btn_start_client" $0
FunctionEnd

; Function OnCountdownTimer
; 	!insertmacro "NSDM_DEBUG_INFO" "������Ϸ����ʱ: $Countdown"

; 	IntOp $Countdown $Countdown - 1
; 	${If} $Countdown == 0
; 		${nsdm.KillTimer} $CountdownTimer
; 		Call OnBtnStartGame
; 	${Else}
; 		StrCpy $0 "$Countdown s���Զ�������Ϸ����"
; 		${nsdm.SetControlAttr} "label_counter_start" "$0" "text"
; 	${EndIf}
; FunctionEnd

Function OnShufflingFigureTimer 
	!insertmacro "NSDM_DEBUG_INFO" "�ֲ�ͼ��ʱ�� $Countdown"
	IntOp $Countdown $Countdown + 1
	${If} $Countdown > 3
		StrCpy $Countdown 1
	${EndIf}
	${nsdm.SetControlAttr} "wnd_shuffling_figure" "shuffling_figure$Countdown" "skin"
FunctionEnd

Function OnInstallTimer
	; ���ͻ��˽����Ƿ�������
	!insertmacro "ProcessDetect" "${PRODUCT_CLIENT_EXE}" ";" "$INSTDIR"
	Pop $0
	${If} $0 == 0
	    # Ŀ�����û������
		${nsdm.KillTimer} $InstallTimer
		!insertmacro "NSDM_DEBUG_INFO" "����û�������У�ֱ�ӽ��밲װ����"
		Call OnBtnClickInstall
	${Else}
		IntOp $CheckClientCount $CheckClientCount + 1
		!insertmacro "NSDM_DEBUG_INFO" "�����������У��ȴ�����$CheckClientCount"
		${If} $CheckClientCount >= 3
			#  �ȴ���ʱ
			${nsdm.KillTimer} $InstallTimer
			!insertmacro "NSDM_DEBUG_INFO" "�ȴ�ָ�����̽�����ʱ"
			!insertmacro PostPasEvent ${PAS_INSTALL_FAILED_TIMEOUT}
			Call OnBtnStartGame
		${EndIf}
	${EndIf}
FunctionEnd

Function TerminateIfExists
	Pop $R2 ; ����������

	!insertmacro "ProcessDetect" "$R2" ";" ""
	Pop $0
	${If} $0 == 0
		Push 1
		Return
	${EndIf}

	${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "��ʾ" "label_popup_content" "��⵽${PRODUCT_NAME}�������У����[ȷ��]ǿ�ƽ������̣�" ${MB_OKCANCEL} $0
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
			${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "��ʾ" "label_popup_content" "��⵽${PRODUCT_NAME}�������У��޷�ǿ�ƽ������̣�" ${MB_OKCANCEL} $0
			Push 0
		${Else}
			Push 1
		${EndIf}
	${Else}
		Push 0
	${EndIf}
FunctionEnd

# ==================== ж������ ================================
Function un.UninstallFunc
	; ���ͻ��˽����Ƿ�������
	!insertmacro "NSDM_DEBUG_INFO" "${PRODUCT_CLIENT_EXE}"
	Push "${PRODUCT_CLIENT_EXE}"
	Call un.TerminateIfExists
	Pop $0
	${If} $0 == 0
		Return
	${ElseIf} $0 == -1
		${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "��ʾ" "label_popup_content" "�޷�ǿ�ƽ���'${PRODUCT_CLIENT_EXE}'�����ֶ��˳���" ${MB_OKCANCEL} $R0
		Return
	${EndIf}

	!insertmacro "NSDM_DEBUG_INFO" "��ʼж��"
	${nsdm.SetControlAttr} "btn_close" 1 "bdisable"
	${nsdm.SelectPage} "wnd_uninstallling"

	; ɾ��ע�����Ϣ
	SetRegView 32
	DeleteRegKey ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}"
	DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
	IntOp $UninstallProgress 0 + 1
	${nsdm.SetControlAttr} "progress_uninstalling" $UninstallProgress "value"

	; ɾ����ݷ�ʽ
	Call un.DeleteShortCut
	IntOp $UninstallProgress $UninstallProgress + 2
	${nsdm.SetControlAttr} "progress_uninstalling" $UninstallProgress "value"

	; ��̨ɾ���ļ�
	GetFunctionAddress $0 un.RemoveFiles
	BgWorker::CallAndWait

	${nsdm.SelectPage} "wnd_uninstalled"
	
	Delete "$INSTDIR\${PRODUCT_UNINSTALL_NAME}"
	; RMDir /r "$INSTDIR"
	RMDir "$INSTDIR" ; �ǿղ�ɾ��

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
        ; ʹ�ò��ɾ��������ͼ�꣬��֧��vistaϵͳ
        StrCpy $R1 "$QUICKLAUNCH\User Pinned\TaskBar"
        ${StdUtils.InvokeShellVerb} $0 "$R1" "${PRODUCT_NAME}.lnk" ${StdUtils.Const.ShellVerb.UnpinFromTaskbar}
    ${Else}
    	Delete "$QUICKLAUNCH\${PRODUCT_NAME}.lnk"
  	${EndIf}

	Delete "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk"
	Delete "$SMPROGRAMS\${PRODUCT_NAME}\ж��${PRODUCT_NAME}.lnk"
  	Delete "$DESKTOP\${PRODUCT_NAME}.lnk" ; ����ͼ��
  	Delete "$SMSTARTUP\${PRODUCT_NAME}.lnk" ; ����������
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

	!insertmacro NSDM_DEBUG_INFO "��ʼ��DM��"
	${nsdm.InitSkinEngine} ${DM_SKIN_PATH} "dui_uninstall_wnd" "uninstall_frame" $NSDM_OUT

	!insertmacro NSDM_DEBUG_INFO "$NSDM_OUT"
	Call un.BindUnInstUIControls
	Call un.InitDialog

	!insertmacro PostPasEvent ${PAS_BEGIN_UNINSTALL}

	${nsdm.Run}
	!insertmacro NSDM_DEBUG_INFO "ж��DM��"

	!insertmacro PostSessionData
	${nscurl.WaitDone} 5000 $0
FunctionEnd

Function un.InitDialog
	${nsdm.SetControlAttr} "label_install_title" "${PRODUCT_DESCRIPTION}${PRODUCT_VERSION}" "text"
FunctionEnd

Function un.onInit
	; ���ж�ؽ����Ƿ�������
	!insertmacro "CheckSetupRunning" ${SETUP_MUTEX_NAME} $NSDM_OUT
	${If} $NSDM_OUT == 1
		MessageBox MB_OK|MB_ICONEXCLAMATION "��װ��ж�س����Ѿ������С�"
   		Abort
	${EndIf}

	InitPluginsDir
	File "/ONAME=${DM_SKIN_PATH}" "..\skin\skin.zip"

	!insertmacro SetSessionId
	!insertmacro PostSessionData
FunctionEnd

Function un.BindUnInstUIControls
	; ȫ��
	GetFunctionAddress $0 "un.OnExitDUISetup"
	${nsdm.RegisterCommandNotify} "btn_close" $0

	; ж�ع��̴���
	GetFunctionAddress $0 "un.OnExitDUISetup"
	${nsdm.RegisterCommandNotify} "btn_uninstall_cancel" $0

	GetFunctionAddress $0 "un.UninstallFunc"
	${nsdm.RegisterCommandNotify} "btn_uninstall_ok" $0

	; ж����ɴ���
	GetFunctionAddress $0 "un.OnBtnCommit"
	${nsdm.RegisterCommandNotify} "btn_commit" $0

	GetFunctionAddress $0 "un.OnBtnDownload"
	${nsdm.RegisterCommandNotify} "btn_download_client" $0
FunctionEnd

Function un.OnBtnCommit
	; ����������
	${nsdm.SetWindowAttr} "0" "bvisible"

	; �ϱ�ж�ط���
	Call un.BuildUserFeedback

	; �˳�
	Call un.OnExitDUISetup
FunctionEnd

Function un.BuildUserFeedback
	; ��ȡ�û�������Ϣ
	${nsdm.GetControlAttr} "btn_reason_reinstall" "check" $1
	${nsdm.GetControlAttr} "btn_reason_no_love" "check" $2
	${nsdm.GetControlAttr} "btn_reason_no_game" "check" $3
	${nsdm.GetControlAttr} "btn_reason_performance" "check" $4
	${nsdm.GetControlAttr} "edit_reason" "text" $5
	!insertmacro "NSDM_DEBUG_INFO" "ж��ԭ�� $5"
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
	Pop $R2 ; ����������

	!insertmacro "ProcessDetect" "$R2" ";" ""
	Pop $0
	${If} $0 == 0
		Push 1
		Return
	${EndIf}

	${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "��ʾ" "label_popup_content" "��⵽${PRODUCT_NAME}�������У����[ȷ��]ǿ�ƽ������̣�" ${MB_OKCANCEL} $0
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
			${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "��ʾ" "label_popup_content" "��⵽${PRODUCT_NAME}�������У��޷�ǿ�ƽ������̣�" ${MB_OKCANCEL} $0
			Push 0
		${Else}
			Push 1
		${EndIf}
	${Else}
		Push 0
	${EndIf}
FunctionEnd

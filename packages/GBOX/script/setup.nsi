# ====================== ��װ�����ʼ���峣�� ==============================
; !define _DEBUG 1
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
;ReserveFile "..\addon\vcredist_x86.exe"
;ReserveFile "..\addon\license.txt"
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

# ====================  Test =====================================

# ==================== �Զ���ҳ�� ================================
Page custom DMPage

# UninstPage custom un.DMPage

# ==================== ȫ�ֱ��� ================================
!define DM_SKIN_PATH "$PLUGINSDIR\skin.zip"
!define GAME_LICENSE_PATH "$PLUGINSDIR\license.txt"
!define VC_REDIST_PATH "$PLUGINSDIR\vcredist_x86.exe"
;!define NSDM_OUT $4
;!define INSTALL_SIZE 12582912 ;"12 MB"
!define MB_OKCANCEL 1
!define MB_RETRYCANCEL 5
!define IDOK 1
!define IDCANCEL 2
;!define IDRETRY 4
!define IDCLOSE 8
Var InstallState ; ��װ״̬(doing��done)
Var CountdownTimer ; ��װ��ɽ��浹��ʱ��ʱ��
Var Countdown ; ����ʱ����
;Var UninstallProgress ; ж�ؽ���
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
	; ���ͷ�Launcher����
	File "..\addon\glauncher.exe"
	; File /r /x .svn "..\product\*.*"
	SetCompress off
	File "..\product\product.7z"
	SetCompress auto
	SetRebootFlag false

	GetFunctionAddress $R9 ExtractCallback
	Nsis7z::ExtractWithCallback "$OUTDIR\product.7z" $R9
	
	# �ļ���ѹ�п���ʧ�ܣ����ж�Ŀ���ļ��Ƿ����
	IfFileExists "$INSTDIR\${PRODUCT_MAIN_EXE}" done 0
	!insertmacro "NSDM_DEBUG_INFO" "��һ���ͷ��ļ�ʧ�ܣ����Խ��еڶ����ͷ�"
	# ���ﲻ��ת��ȥ����Ϊѭ����תҪ�Ӵ����жϣ�̫�鷳
	GetFunctionAddress $R9 ExtractCallback
	Nsis7z::ExtractWithCallback "$OUTDIR\product.7z" $R9

	IfFileExists "$INSTDIR\${PRODUCT_MAIN_EXE}" done 0
	!insertmacro "NSDM_DEBUG_INFO" "�ڶ����ͷ��ļ�ʧ��"
	Delete "$OUTDIR\product.7z"
	Return
	done:
	!insertmacro "NSDM_DEBUG_INFO" "�ͷ��ļ��ɹ�"
	Delete "$OUTDIR\product.7z"
	# ini�ļ���������̬����
	# File "..\addon\Version.ini"
FunctionEnd

Function InstallFuncSlient
	;����Ҫ�����İ�װ�ļ�
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
	Pop $R8 ; �Ѱ�װ��С
  	Pop $R9 ; �ܴ�С
  	System::Int64Op $R8 * 100
  	Pop $R7
  	System::Int64Op $R7 / $R9
  	Pop $R7 ; ��װ�ٷֱ�
  	${If} $R8 == $R9
        ${nsdm.SetControlAttr} "progress_installing" 100 "value"
		${nsdm.SetControlAttr} "label_install_percent" "100%" "text"
    ${Else}
		${nsdm.SetControlAttr} "progress_installing" $R7 "value"
		${nsdm.SetControlAttr} "label_install_percent" "$R7%" "text"
	${EndIf}
FunctionEnd

; ���ɿ�ݷ�ʽ
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

; ������ʼ�˵���ݷ�ʽ
Function CreateStartMenu
	CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" '${PRODUCT_STARTMENU_ARG}'
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\ж��${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_UNINSTALL_NAME}"

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
	File "..\addon\${PRODUCT_UNINSTALL_NAME}"

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

/*
Function SlientInstall
	!insertmacro "NSDM_DEBUG_INFO" "���뾲Ĭ��װ"

	!insertmacro PostPasEvent ${PAS_BEGIN_INSTALL}

	; ����Ƿ��Ѿ���װ�����а汾�������¾�ֱ���˳�

	ReadRegStr $1 ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "version"
	${If} "$1" != ""
		!insertmacro "NSDM_DEBUG_INFO" "�Ѿ���װ�����˳�"
		Goto WaitThread_PostPasEvent
	${EndIf}

	; ���ݾɰ汾
	ReadRegStr $1 "HKLM" "${PRODUCT_SUB_KEY}" "version"
	${If} "$1" != ""
		!insertmacro "NSDM_DEBUG_INFO" "�Ѿ���װ�����˳�"
		Goto WaitThread_PostPasEvent
	${EndIf}

	!insertmacro "NSDM_DEBUG_INFO" "SetLastInstallPath INSTDIR $INSTDIR"
	Call SetLastInstallPath
	!insertmacro "NSDM_DEBUG_INFO" "SetLastInstallPath INSTDIR $INSTDIR"
	
	; �����̿ռ��Ƿ��㹻
	!insertmacro "GetFreeSpaceSize" "$INSTDIR" $R0
	System::Int64Op $R0 > ${INSTALL_SIZE}
	Pop $0
	${If} $0 == 0
		!insertmacro "NSDM_DEBUG_INFO" "���̿ռ䲻�㣬�˳�"
		Goto WaitThread_PostPasEvent
	${EndIf}

	; ���ͻ��˽����Ƿ�������
	!insertmacro "ProcessDetect" "${PRODUCT_CLIENT_EXE}" ";" ""
	Pop $0
	${If} $0 != 0
		!insertmacro "NSDM_DEBUG_INFO" "�Ѿ���һ����װ����ʵ�����˳�"
		Goto WaitThread_PostPasEvent
	${EndIf}

	SetRegView 32
	; ���vcredist
	Call CheckVCRedist
	IfErrors 0 Install_Continue
		!insertmacro "NSDM_DEBUG_INFO" "��ǰϵͳû�м�⵽vc2008 redist"
		;GetFunctionAddress $0 InstallVCRedist
    ;BgWorker::CallAndWait
		Call InstallVCRedist
		IfErrors 0 Install_Continue
			Call HandleVCRedistCancel
			!insertmacro "NSDM_DEBUG_INFO" "��װvc2008 redistʧ�ܣ������˳�"
			Goto WaitThread_PostPasEvent
 	Install_Continue:
  		!insertmacro "NSDM_DEBUG_INFO" "�Ѿ���װvc2008 redist"

	; д��ע�����Ϣ
	WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "installDir" "$INSTDIR"
	WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "version" "${PRODUCT_VERSION}"

 	!insertmacro "NSDM_DEBUG_INFO" "��ѹ�ļ���ָ��Ŀ¼"
  Call InstallFuncSlient

	SetShellVarContext ${PRODUCT_LNK_USER}

	Push "/nodeskicon"
	Call ParameterDetect
	Pop $0
			!insertmacro "NSDM_DEBUG_INFO" "$0"
	${If} $0 == 0
		!insertmacro "NSDM_DEBUG_INFO" "���������ݷ�ʽ"
	  CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" '${PRODUCT_DESKTOP_ARG}'
	${Else}
		!insertmacro "NSDM_DEBUG_INFO" "��⵽��������/nodeskicon�������������ݷ�ʽ"
	${EndIf}

	Push "/notaskbaricon"
	Call ParameterDetect
	Pop $0
		!insertmacro "NSDM_DEBUG_INFO" "$0"
	${If} $0 == 0
		!insertmacro "NSDM_DEBUG_INFO" "������������ݷ�ʽ"
  	Call CreateTaskbar
	${Else}
		!insertmacro "NSDM_DEBUG_INFO" "��⵽��������/notaskbaricon����������������ݷ�ʽ"
	${EndIf}


  !insertmacro "NSDM_DEBUG_INFO" "������ʼ�˵���ݷ�ʽ"
  Call CreateStartMenu

  ${RefreshShellIcons}
  SetShellVarContext current

	Call BuildUninstaller

	StrCpy $InstallState "done"
	!insertmacro PostPasEvent ${PAS_INSTALL_SUCCESS}

	;!insertmacro "NSDM_DEBUG_INFO" "������Ϸ����"
	;Exec '"$INSTDIR\${PRODUCT_MAIN_EXE}" ${PRODUCT_DESKTOP_ARG}'
	;!insertmacro PostPasEvent ${PAS_RUN_CLIENT}
	
	; �����ƻ��������
	; Exec '"$INSTDIR\${PRODUCT_SCHEDULE_EXE}" ""'
	nsutil::NSUpdateSchedule "0EDF91F7-D4D0-4DD7-B739-62067F6683FC" "YY��Ϸ������������" "$INSTDIR\popup" "$INSTDIR\popup\${PRODUCT_SCHEDULE_EXE}" "--type=yyGameOem" "60"


	; ��װ�����������ϱ��͵ȴ��ϱ�����
WaitThread_PostPasEvent:
	!insertmacro "NSDM_DEBUG_INFO" "��װ���������������ϱ�"

	!insertmacro PostSessionData
	${nscurl.WaitDone} 10000 $0

	!insertmacro "NSDM_DEBUG_INFO" "��װ�����������ϱ�����"
FunctionEnd*/

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

	!insertmacro NSDM_DEBUG_INFO "��ʼ��DM��"
	${nsdm.InitSkinEngine} ${DM_SKIN_PATH} "dui_install_wnd" "install_frame" $NSDM_OUT

	Call BindUIControls

	Call InitDialog

	Call CheckInstalled

	${If} $OverwriteInstall == 2
		!insertmacro NSDM_DEBUG_INFO "��װ���汾�Ų����ڱ��ذ汾�ţ�ֱ�������ͻ���"
		StrCpy $InstallState "done"
		Call OnBtnStartGame
	${Else}
		!insertmacro NSDM_DEBUG_INFO "������װ��ʱ����׼�����а�װ"
		${nsdm.Run}
	${EndIf}
	!insertmacro NSDM_DEBUG_INFO "ж��DM��"

	!insertmacro PostSessionData
	${nscurl.WaitDone} 10000 $0
FunctionEnd

Function DMPage
	;!insertmacro NSDM_DEBUG_INFO "�Ǿ�Ĭ��װ"
	Call GUIInstall
FunctionEnd


Function InitDialog
	StrCpy $NSDM_OUT $INSTDIR
	Call SetInstallFolder

	StrCpy $MBSize "640,400"

	${nsdm.SetControlAttr} "btn_checkbox_license" 1 "check"
	${nsdm.SetControlAttr} "btn_quick_taskbar" 0 "check"
	${nsdm.SetControlAttr} "btn_startmenu_lnk" 1 "check"

	;${nsdm.SetControlAttr} "label_install_title" "${PRODUCT_DESCRIPTION}${PRODUCT_VERSION}" "text"

	!insertmacro PostPasEvent ${PAS_BEGIN_INSTALL}
FunctionEnd

; ����Ƿ��Ѿ���װ�ͻ���
Function CheckInstalled
	StrCpy $1 "$INSTDIR\${PRODUCT_MAIN_EXE}"
	StrCpy $2 "$INSTDIR\Version.ini"
	StrCpy $OverwriteInstall 0
	IfFileExists "$1" 0 out
	
	ReadINIStr $3 "$2" "gbox" "NewVersion"
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
		!insertmacro "NSDM_DEBUG_INFO" "��װ��ж�س����Ѿ������У��˳�"
		IfSilent 0 MsgBoxAndAbort
			Abort
		MsgBoxAndAbort:
			;MessageBox MB_OK|MB_ICONEXCLAMATION "��װ��ж�س����Ѿ������С�"
   		Abort
	${EndIf}

	!insertmacro "NSDM_DEBUG_INFO" ".onInit"
	InitPluginsDir
	File "/ONAME=${DM_SKIN_PATH}" "..\skin\skin.zip"
	;File "/ONAME=${GAME_LICENSE_PATH}" "..\addon\license.txt"
	;File "/ONAME=${VC_REDIST_PATH}" "..\addon\vcredist_x86.exe"

	!insertmacro SetSessionId
	!insertmacro PostSessionData

	;IfSilent 0 NonSlientInstall
		;!insertmacro NSDM_DEBUG_INFO "��Ĭ��װ"
		;Call SlientInstall
	;NonSlientInstall:
FunctionEnd

; ��װ�������˳���������ʾ
Function OnExitDUISetup
	!insertmacro "NSDM_DEBUG_INFO" "�����˳�����ǰ��װ״̬Ϊ��$InstallState"
	${If} $InstallState == "doing"
		${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "��ʾ" "label_popup_content" "ȷ���˳���װ��" ${MB_OKCANCEL} $NSDM_OUT
		!insertmacro "NSDM_DEBUG_INFO" "$NSDM_OUT"
		${If} $NSDM_OUT != ${IDOK}
			Return
		${Else}
			!insertmacro PostPasEvent ${PAS_INSTALL_CANCELED}
		${EndIf}
	${EndIf}
	${nsdm.KillTimer} $CountdownTimer
	${nsdm.UninitSkinEngine}
	!insertmacro PostPasEvent ${PAS_END_INSTALL}
FunctionEnd

Function OnBtnMin
  ${nsdm.SendMessage} "UM_NSDM_MIN" "" ""
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

		${nsdm.SetControlAttr} "edit_install_folder" "$NSDM_OUT" "text"

		;!insertmacro "FormatSizeString" ${ProuctINSTALL_SIZE} $R0
		;StrCpy $R1 "����ռ�: $R0"
		;${nsdm.SetControlAttr} "label_install_size" $R1 "text"

		;!insertmacro "GetFreeSpaceSize" "$NSDM_OUT" $R0
		;!insertmacro "FormatSizeString" $R0 $R0
		;StrCpy $R1 "���̿��ÿռ�: $R0"
		;${nsdm.SetControlAttr} "label_useable_size" $R1 "text"

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
	${nsdm.GetControlAttr} "edit_install_folder" "text" $NSDM_OUT
	Call SetInstallFolder

	;�������յİ�װ·��
	;${If} $OverwriteInstall == 0
		; �Ǹ��ǰ�װģʽ�£�����������·��
		;Call SetLastInstallPath
	;${EndIf}


	; �����̿ռ��Ƿ��㹻
	!insertmacro "GetFreeSpaceSize" "$NSDM_OUT" $R0
	System::Int64Op $R0 > ${PRODUCT_INSTALL_SIZE}
	Pop $0
	${If} $0 == 0
		!insertmacro "NSDM_DEBUG_INFO" "$NSDM_OUT���ÿռ�$R0���ͻ�����Ҫ�ռ�${PRODUCT_INSTALL_SIZE}"
		!insertmacro PostPasEvent ${PAS_INSTALL_FAILED_DESKSPACE}
		${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "������ʾ" "label_popup_content" "���̿ռ䲻�㣬��ѡ����������!" ${MB_OKCANCEL} $0
		Return
	${EndIf}

	; ���ͻ��˽����Ƿ�������
	Push "${PRODUCT_CLIENT_EXE}"
	Call TerminateIfExists
	Pop $0
	${If} $0 == 0
		Return
	${EndIf}

	; ������չ����
	Call HideCustomWnd

	; �ж��Ƿ��Զ��尲װ
	${nsdm.GetControlAttr} "edit_install_folder" "text" $NSDM_OUT
	StrCmp "$INSTDIR" "$NSDM_OUT" 0 begin_custom_install
	!insertmacro PostPasEvent ${PAS_INSTALL_DEFAULT}
	Goto end_custom_install
 begin_custom_install:
	!insertmacro PostPasEvent ${PAS_INSTALL_CUSTOM}
 end_custom_install:

	${nsdm.SelectPage} "wnd_installling"

	SetRegView 32
	; ���vcredist
	;Call CheckVCRedist
	;IfErrors 0 Install_Continue
	;!insertmacro "NSDM_DEBUG_INFO" "��ǰϵͳû�м�⵽vc2008 redist"
	;GetFunctionAddress $0 InstallVCRedist
    ;BgWorker::CallAndWait
	;IfErrors 0 Install_Continue
	;Call HandleVCRedistCancel
	;!insertmacro "NSDM_DEBUG_INFO" "��װvc2008 redistʧ�ܣ������˳�"
	;Return
 ;Install_Continue:
 	;!insertmacro "NSDM_DEBUG_INFO" "�Ѿ���װvc2008 redist"

	; д��ע�����Ϣ
	WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "installDir" "$INSTDIR"
	WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "version" "${PRODUCT_VERSION}"

	${nsdm.SetControlAttr} "btn_close" 1 "bdisable" ; �ͷ��ļ��н�ֹ�˳�
	; ����һ�������ȼ��ĺ�̨�߳�
    GetFunctionAddress $0 InstallFunc
    BgWorker::CallAndWait
    ${nsdm.SetControlAttr} "btn_close" 0 "bdisable"

	IfFileExists "$INSTDIR\${PRODUCT_MAIN_EXE}" unpack_success 0
	!insertmacro "NSDM_DEBUG_INFO" "�ļ���ѹʧ�ܣ��˳���װ"
	!insertmacro PostPasEvent ${PAS_INSTALL_FAILED_UNPACK}
	StrCpy $InstallState "done"
	Call OnBtnStartGame
	Return

unpack_success:
	${nsdm.SetControlAttr} "label_counter_start" "3 s���Զ�����GBox�����" "text"
	${nsdm.SelectPage} "wnd_installed"

	StrCpy $Countdown 3
	GetFunctionAddress $CountdownTimer OnCountdownTimer
	${nsdm.CreateTimer} $CountdownTimer 1000

	Call BuildShortCut
	Call BuildUninstaller

	StrCpy $InstallState "done"
	!insertmacro PostPasEvent ${PAS_INSTALL_SUCCESS}
	!insertmacro PostPasEvent ${PAS_INSTALL_CHANNEL}
	
	ReadINIStr $1 "$INSTDIR\Version.ini" "gbox" "NewVersion"
	${If} "$1" == ""
		WriteINIStr "$INSTDIR\Version.ini" "gbox" "NewVersion" "${PRODUCT_VERSION}"
		WriteINIStr "$INSTDIR\Version.ini" "gbox" "OldVersion" "${PRODUCT_VERSION}"
		WriteINIStr "$INSTDIR\Version.ini" "gbox" "Version" "${PRODUCT_VERSION}"
	${Else}
		WriteINIStr "$INSTDIR\Version.ini" "gbox" "NewVersion" "${PRODUCT_VERSION}"
		WriteINIStr "$INSTDIR\Version.ini" "gbox" "Version" "${PRODUCT_VERSION}"
		WriteINIStr "$INSTDIR\Version.ini" "gbox" "OldVersion" "$1"
	${EndIf}

	WriteINIStr "$INSTDIR\Config.ini" "pas" "channel" "${PRODUCT_CHANNEL_NAME}"
	WriteINIStr "$INSTDIR\Config.ini" "pas" "channeltag" "${PRODUCT_CHANNEL_TAG}"
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
	!insertmacro "NSDM_DEBUG_INFO" "HideCustomWnd"
	StrCpy $MBSize "640,400"
	${nsdm.SetControlAttr} "btn_checkbox_custom_install" "�Զ��尲װ" "text"
	${nsdm.SetWindowAttr} "$MBSize" "possize"
FunctionEnd

Function ShowCustomWnd
	!insertmacro "NSDM_DEBUG_INFO" "ShowCustomWnd"
	StrCpy $MBSize "640,540"
	${nsdm.SetControlAttr} "btn_checkbox_custom_install" "����" "text"
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
	
	GetFunctionAddress $0 "OnBtnMin"
	${nsdm.RegisterCommandNotify} "btn_min" $0

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

Function OnCountdownTimer
	!insertmacro "NSDM_DEBUG_INFO" "������Ϸ����ʱ: $Countdown"

	IntOp $Countdown $Countdown - 1
	${If} $Countdown == 0
		${nsdm.KillTimer} $CountdownTimer
		Call OnBtnStartGame
	${Else}
		StrCpy $0 "$Countdown s���Զ�����GBox�����"
		${nsdm.SetControlAttr} "label_counter_start" "$0" "text"
	${EndIf}
FunctionEnd

Function TerminateIfExists
	Pop $R2 ; ����������

	!insertmacro "ProcessDetect" "$R2" ";" "$INSTDIR"
	Pop $0
	${If} $0 == 0
		Push 1
		Return
	${EndIf}

	${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "��ʾ" "label_popup_content" "��⵽${PRODUCT_NAME}�������У����[ȷ��]ǿ�ƽ������̣�" ${MB_OKCANCEL} $0
	${If} $0 == ${IDOK}
		${Do}
			!insertmacro "ExeEnd" "$R2" ";" "$INSTDIR"
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



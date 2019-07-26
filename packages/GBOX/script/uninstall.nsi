# ====================== ��װ�����ʼ���峣�� ==============================
;!define _DEBUG 1
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
!include "PostExec.nsh"
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
;Page custom DMPage

UninstPage custom un.DMPage

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
Var MessageBoxOkTimer ; ж����ɽ��浹��ʱ��ʱ��
;Var Countdown ; ����ʱ����
Var UninstallProgress ; ж�ؽ���
;Var InstallRoot ; ��װ��Ŀ¼
Var PasSessionId ; pas�ϱ�session id
Var MBSize ; �������С
;Var OverwriteInstall ; �Ƿ񸲸ǰ�װ: 0 δ��⵽�Ѱ�װ�汾; 1 ���ǰ�װ; 2 ȡ����װ
Var NSDM_OUT

OutFile    "uninst_build.exe"             ;��װ������

; ����ж�س���
;!finalize 'call "%1" & del "%1"'          ;ִ�����ɵ�uninst_build.exe����֧��nsis3
; ${PostExec2} "wscript.exe" "$EXEDIR\..\..\..\script\uac.vbs"

Section "-Dummy"
	; ����uninstall.exe�ļ��������õ���ǰĿ¼��
	WriteUninstaller "$EXEDIR\..\addon\${PRODUCT_UNINSTALL_NAME}"
	;Nop
SectionEnd

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
	${nsdm.SetControlAttr} "label_install_percent" "$UninstallProgress%" "text"

	; ɾ����ݷ�ʽ
	Call un.DeleteShortCut
	IntOp $UninstallProgress $UninstallProgress + 2
	${nsdm.SetControlAttr} "progress_uninstalling" $UninstallProgress "value"
	${nsdm.SetControlAttr} "label_install_percent" "$UninstallProgress%" "text"

	${nsdm.SetControlAttr} "btn_close" 1 "bdisable" ; �ͷ��ļ��н�ֹ�˳�
	; ��̨ɾ���ļ�
	GetFunctionAddress $0 un.RemoveFiles
	BgWorker::CallAndWait

	;${nsdm.SelectPage} "wnd_uninstalled"
	
	Delete "$INSTDIR\${PRODUCT_UNINSTALL_NAME}"
	; RMDir /r "$INSTDIR"
	RMDir "$INSTDIR" ; �ǿղ�ɾ��
	${nsdm.SetControlAttr} "btn_close" 0 "bdisable" ; �ͷ��ļ��н�ֹ�˳�

	!insertmacro PostPasEvent ${PAS_UNINSTALL_SUCCESS}
	
	${nsdm.SetControlAttr} "label_install_percent" "100%" "text"
	GetFunctionAddress $MessageBoxOkTimer un.OnMessageBoxOkTimer
	${nsdm.CreateTimer} $MessageBoxOkTimer 1000
	;${nsdm.MessageBox} "dui_message_dlg" "$MBSize" "label_popup_title" "��ʾ" "label_popup_content" "ж�سɹ���" ${MB_OKCANCEL} $0
FunctionEnd

Function un.OnMessageBoxOkTimer
	${nsdm.SelectPage} "wnd_uninstall_ok"
	${nsdm.SetControlAttr} "logo" "0" "bvisible"
	${nsdm.SetControlAttr} "label_install_title" "0" "bvisible"
	${nsdm.SetControlAttr} "btn_close" "0" "bvisible"
	${nsdm.SetControlAttr} "btn_min" "0" "bvisible"
	
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
	${nsdm.SetControlAttr} "label_install_percent" "100%" "text"
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
	${nsdm.SetControlAttr} "label_install_percent" "$UninstallProgress%" "text"

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
	StrCpy $MBSize "640,400"
	;${nsdm.SetControlAttr} "label_install_title" "${PRODUCT_DESCRIPTION}${PRODUCT_VERSION}" "text"
FunctionEnd

Function un.onInit
	; ���ж�ؽ����Ƿ�������
	!insertmacro "CheckSetupRunning" ${SETUP_MUTEX_NAME} $NSDM_OUT
	${If} $NSDM_OUT == 1
		MessageBox MB_OK|MB_ICONEXCLAMATION "��װ��ж�س����Ѿ������С�"
   		Abort
	${EndIf}

	!insertmacro "NSDM_DEBUG_INFO" "un.onInit"
	
	InitPluginsDir
	File "/ONAME=${DM_SKIN_PATH}" "..\skin\skin.zip"

	!insertmacro SetSessionId
	!insertmacro PostSessionData
FunctionEnd

Function un.BindUnInstUIControls
	; ȫ��
	GetFunctionAddress $0 "un.OnExitDUISetup"
	${nsdm.RegisterCommandNotify} "btn_close" $0
	
	GetFunctionAddress $0 "un.OnBtnMin"
	${nsdm.RegisterCommandNotify} "btn_min" $0

	; ж�ع��̴���
	GetFunctionAddress $0 "un.OnExitDUISetup"
	${nsdm.RegisterCommandNotify} "btn_uninstall_cancel" $0

	GetFunctionAddress $0 "un.UninstallFunc"
	${nsdm.RegisterCommandNotify} "btn_uninstall_ok" $0

	; ж����ɴ���
	GetFunctionAddress $0 "un.OnBtnCommit"
	${nsdm.RegisterCommandNotify} "btn_ok2" $0

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
	${nsdm.GetControlAttr} "btn_reason_hinder_gaming" "check" $2
	${nsdm.GetControlAttr} "btn_reason_useless" "check" $3
	${nsdm.GetControlAttr} "btn_reason_awkward" "check" $4
	${nsdm.GetControlAttr} "btn_reason_ugly" "check" $5
	${nsdm.GetControlAttr} "btn_reason_performance" "check" $6
	${nsdm.GetControlAttr} "edit_reason" "text" $7
	${nsdm.GetControlAttr} "edit_contact_info" "text" $8
	!insertmacro "NSDM_DEBUG_INFO" "ж��ԭ�� $7"
	nsutil::NSUrlEncode "$7"
	Pop $7
	nsutil::NSUrlEncode "$8"
	Pop $8

	StrCpy $R0 "http://lobbydata.game.yy.com/uninstall/add.do?product=${PRODUCT_NAME}&reasonType="
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
		StrCpy $R0 "$R04|"
	${EndIf}
	${If} $5 == 1
		StrCpy $R0 "$R05|"
	${EndIf}
	${If} $6 == 1
		StrCpy $R0 "$R06"
	${EndIf}
	StrCpy $R0 "$R0&reasonContext=$7"
	StrCmp $8 "" +2 0
	StrCpy $R0 "$R0($8)"

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

Function un.OnBtnMin
	${nsdm.SendMessage} "UM_NSDM_MIN" "" ""
FunctionEnd

Function un.TerminateIfExists
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

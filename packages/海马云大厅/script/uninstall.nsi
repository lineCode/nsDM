# ====================== ��װ�����ʼ���峣�� ==============================
#!define _DEBUG 1
;SetCompressor lzma
SetCompress off
!packhdr "$%TEMP%\yygame_exehead.tmp" '"..\..\..\script\upx.exe" -9 "$%TEMP%\yygame_exehead.tmp"'

# ====================== ��Ʒ�Զ�����Ϣ ==============================
!include "product.nsh"

# ===================== ȫ������ =============================
!ifndef _DEBUG
	!verbose 3 ; �ӿ�����ٶ�
!endif

# ===================== MUI���� =============================
!include "MUI.nsh"


!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS ; ��װԤ�ͷ��ļ�

# ===================== ���������ͷ�ļ��Ͳ�� =============================
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

# ==================== NSIS���� ================================
BrandingText /TRIMLEFT "${PRODUCT_PUBLISHER}"
SetFont "΢���ź�" 8
CRCCheck on
InstallDirRegKey ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "installDir" ; ��ע����ȡ��װ·��
RequestExecutionLevel user ; Ȩ������ none|user|highest|admin
Icon "..\skin\install.ico" ; ��װͼ��
UninstallIcon "..\skin\uninstall.ico" ; ж��ͼ��
ShowInstDetails hide ; �����Ƿ���ʾ��װ��ϸ��Ϣnevershow
ShowUninstDetails hide ; �����Ƿ���ʾж����ϸ��Ϣnevershow
!insertmacro MUI_LANGUAGE "SimpChinese"
SilentInstall silent ; ��Ĭ��װ
;SilentUnInstall silent ; ��Ĭж��


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

UninstPage custom un.DMPage

# ==================== ȫ�ֱ��� ================================
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
Var InstallState ; ��װ״̬(doing��done)
;Var CountdownTimer ; ��װ��ɽ��浹��ʱ��ʱ��
;Var Countdown ; ����ʱ����
Var UninstallProgress ; ж�ؽ���
;Var InstallRoot ; ��װ��Ŀ¼
Var PasSessionId ; pas�ϱ�session id
Var MBSize ; �������С
;Var OverwriteInstall ; �Ƿ񸲸ǰ�װ: 0 δ��⵽�Ѱ�װ�汾; 1 ���ǰ�װ; 2 ȡ����װ

OutFile    "uninst_build.exe"             ;��װ������

;!finalize 'call "%1" & del "%1"'          ;ִ�����ɵ�uninst_build.exe
	
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
	
	Sleep 1000

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

	; ɾ���ƻ�����
	nsutil::NSDeleteSchedule "0EDF91F7-D4D0-4DD7-B739-62067F6683FC"

	; ��̨ɾ���ļ�
	GetFunctionAddress $0 un.RemoveFiles
	BgWorker::CallAndWait

	${nsdm.SelectPage} "wnd_uninstalled"
	
	Delete "$INSTDIR\${PRODUCT_UNINSTALL_NAME}"
	; RMDir /r "$INSTDIR"
	Sleep 200
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
	${nsdm.InitSkinEngine} ${DM_SKIN_PATH} "dui_uninstall_wnd" "uninstall_frame" ${NSDM_OUT}

	!insertmacro NSDM_DEBUG_INFO "${NSDM_OUT}"
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
	!insertmacro "CheckSetupRunning" ${SETUP_MUTEX_NAME} ${NSDM_OUT}
	${If} ${NSDM_OUT} == 1
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

	${nscurl.Get} $R0 ${NSDM_OUT}
	!insertmacro "NSDM_DEBUG_INFO" "${NSDM_OUT}"
FunctionEnd

Function un.OnBtnDownload
	${nsdm.SendMessage} "UM_NSDM_OPEN_URL" "http://policyres.game.yy.com/policy/yygame/YYGameSetup_HMY5218.exe" ""
	!insertmacro PostPasEvent ${PAS_DOWNLOAD_CLIENT}
	Call un.OnExitDUISetup
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

	!insertmacro "ProcessDetect" "$R2" ";"
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

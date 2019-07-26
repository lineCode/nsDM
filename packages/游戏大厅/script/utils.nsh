!ifndef __UTILS_NSH__
!define __UTILS_NSH__

!include "FileFunc.nsh"
!include "LogicLib.nsh"

!macro MutexCheck _mutexname _outvar _handle
System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${_mutexname}" ) i.r1 ?e'
StrCpy ${_handle} $1
Pop ${_outvar}
!macroend

!define NSDM_DEBUG `System::Call kernel32::OutputDebugString(ts)`
!macro NSDM_DEBUG_INFO _Content
	!ifdef _DEBUG
		${NSDM_DEBUG} "${__FILE__}:${__LINE__} ${_Content}$\n"
	!else
		Nop
	!endif
!macroend

!macro GetFreeSpaceSize path out
	${GetRoot} ${path} $0
	
	System::Call kernel32::GetDiskFreeSpaceEx(tr0,*l,*l,*l.r0)
	System::Int64Op $0 + 0
	Pop ${out}
!macroend

; ����byteΪ��λ����ֵ����ʽ��Ϊ���ʵ�λ���ַ���
!macro FormatSizeString size out
	; ��$0��$1����ֵ
	System::Int64Op ${size} + 0
	Pop $0
	StrCpy $1 "Bytes"

	${If} $0 > 1024
	${OrIf} $0 < 0
		System::Int64Op $0 / 1024
		Pop $0
		StrCpy $1 "KB"
		${If} $0 > 1024
		${OrIf} $0 < 0
	 		System::Int64Op $0 / 1024
	 		Pop $0
	 		StrCpy $1 "MB"
	 		${If} $0 > 1024
	 		${OrIf} $0 < 0
	    		System::Int64Op $0 / 1024
	    		Pop $0
	    		StrCpy $1 "GB"
	 		${EndIf}
	  	${EndIf}
	${EndIf}

	StrCpy ${out} "$0$1"
!macroend

; �жϰ�װ/ж�ؽ����Ƿ�������
!macro CheckSetupRunning mutex_name out
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${mutex_name}") i .r1 ?e'
	Pop $R0
	IntOp ${out} 0 + 0
	${If} $R0 == 183 ;ERROR_ALREADY_EXISTS
		IntOp ${out} ${out} + 1
	${Else}
		IntOp ${out} ${out} + 0
	${EndIf}
!macroend

; ����exe���̣����ж�����̣���${flag}�ָ�
!macro ExeEnd  exestr flag
; $0 �ܳ�
; $1 ����
; $2 Ŀ���ַ���
; $3 ��ʱ�Ƚϱ���
; $4 �ָ�������
; $5 ����ֵ 1 �ҵ����ɹ���������; 0 û���ҵ�����; -1 �ҵ����̵��ǽ���ʧ��
	StrLen $0 "${exestr}"
	IntOp $1 0 + 0
	StrCpy $2 ""
	StrLen $4 "${flag}"
	StrCpy $5 0

	${ForEach} $1 0 $0 + 1
		StrCpy $3 "${exestr}" $4 $1
		${If} $3 != "${flag}"
			; ƴ�ӵ�ǰ������
			StrCpy $2 "$2$3"
			; ���һ��ѭ��������ҪContinue
			${If} $1 != $0
				${Continue}
			${EndIf}
		${EndIf}

		!insertmacro "NSDM_DEBUG_INFO" "ExeEnd->$2"
		; ȡ�������Ƿָ�������ʾ$2�д洢���Ѿ���һ��·������
		nsutil::NSFindProcess "$2" ""
		Pop $R0
		${If} $R0 <> 1
			; ��ǰ���̲����ڻ����ѱ�������Ѱ����һ������
			StrCpy $2 ""
			${Continue}
		${Else}
			StrCpy $5 1
		${EndIf}

		nsutil::NSKillProcess "$2" ""
		Pop $R0
		${If} $R0 == 1
			StrCpy $5 1
		${Else}
			StrCpy $5 -1
			${Break}
		${EndIf}
	${Next}
	Push $5
!macroend

; ���exe���̣����ж�����̣���${flag}�ָ�
!macro ProcessDetect  exestr flag
; $0 �ܳ�
; $1 ����
; $2 Ŀ���ַ���
; $3 ��ʱ�Ƚϱ���
; $4 �ָ�������
; $5 ����ֵ 1 �ҵ�����; 0 û���ҵ�����;
	StrLen $0 "${exestr}"
	IntOp $1 0 + 0
	StrCpy $2 ""
	StrLen $4 "${flag}"
	StrCpy $5 0

	${ForEach} $1 0 $0 + 1
		StrCpy $3 "${exestr}" $4 $1
		${If} $3 != "${flag}"
			; ƴ�ӵ�ǰ������
			StrCpy $2 "$2$3"
			; ���һ��ѭ��������ҪContinue
			${If} $1 != $0
				${Continue}
			${EndIf}
		${EndIf}

		!insertmacro "NSDM_DEBUG_INFO" "ProcessDetect->$2"
		; ȡ�������Ƿָ�������ʾ$2�д洢���Ѿ���һ��·������
		nsutil::NSFindProcess "$2" ""
		Pop $R0
		${If} $R0 <> 1
			; ��ǰ���̲����ڻ����ѱ�������Ѱ����һ������
			StrCpy $2 ""
			${Continue}
		${Else}
			StrCpy $5 1
			${Break}
		${EndIf}
	${Next}
	Push $5
!macroend

; ˢ��ͼ�껺��
; !macro RefreshShellIcons
;   	; SHCNE_ASSOCCHANGED 0x08000000
;   	; SHCNF_IDLIST 0
;   	System::Call 'shell32.dll::SHChangeNotify(i, i, i, i) v \
;   	(0x08000000, 0, 0, 0)'
; !macroend

;Call CreateGUID
;Pop $0 ;contains GUID
!macro CreateGUID
	Push $0
	Push $1
	Push $2
	System::Alloc 80 
	System::Alloc 16 
	System::Call 'ole32::CoCreateGuid(i sr1) i' 
	System::Call 'ole32::StringFromGUID2(i r1, i sr2, i 80) i' 
	System::Call 'kernel32::WideCharToMultiByte(i 0, i 0, i r2, \
	            i 80, t .r0, i ${NSIS_MAX_STRLEN}, i 0, i 0) i' 
	System::Free $1 
	System::Free $2
	Pop $2
	Pop $1
	Exch $0
!macroend

!macro SetSessionId
	nsutil::NSCreateGuid
	Pop $R0
	; WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "session" "$R0"
	StrCpy $PasSessionId "$R0"
!macroend

!macro BuildSessionUrl
	nsutil::NSGetMachineId
	Pop $1 ; mid
	${GetExeName} $2
	!insertmacro "NSDM_DEBUG_INFO" "ExeName: $2"
	nsutil::NSGetIve "$2"
	Pop $2 ; ive 

	StrCpy $9 "${PAS_SESSION_DATA}&session_id=$PasSessionId&dty=pas&mid=$1&pro=${PAS_PRODUCT_NAME}&cha=${PRODUCT_CHANNEL_NAME}&rso=${PRODUCT_RSO_NAME}&ive=$2"
	nsutil::NSUrlEncode "$9"
	Pop $9
	StrCpy $8 "${PAS_ROOT_URL}?$9"
	Push $8
!macroend

!macro PostSessionData
	!insertmacro BuildSessionUrl
	Pop $8
	${nscurl.GetAsync} $8 $8
	!insertmacro "NSDM_DEBUG_INFO" "PostSessionData: $8"
!macroend

!macro PostPasEvent evt_id evt_desc
	StrCpy $9 "${PAS_EVENT_DATA}&dty=pas&pro=${PAS_PRODUCT_NAME}&session_id=$PasSessionId&eid=${evt_id}&eid_desc=${evt_desc}"
	nsutil::NSUrlEncode "$9"
	Pop $9
	StrCpy $8 "${PAS_ROOT_URL}?$9"
	${nscurl.GetAsync} $8 $8
	!insertmacro "NSDM_DEBUG_INFO" "PostPasEvent: $8"
!macroend

!endif # __UTILS_NSH__


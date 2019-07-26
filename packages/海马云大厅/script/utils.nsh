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

; 将以byte为单位的数值，格式化为合适单位的字符串
!macro FormatSizeString size out
	; 给$0和$1赋初值
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

; 判断安装/卸载进程是否在运行
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

; 结束exe进程，如有多个进程，以${flag}分隔
!macro ExeEnd  exestr flag
; $0 总长
; $1 索引
; $2 目标字符串
; $3 临时比较变量
; $4 分隔符长度
; $5 返回值 1 找到并成功结束进程; 0 没有找到进程; -1 找到进程但是结束失败
	StrLen $0 "${exestr}"
	IntOp $1 0 + 0
	StrCpy $2 ""
	StrLen $4 "${flag}"
	StrCpy $5 0

	${ForEach} $1 0 $0 + 1
		StrCpy $3 "${exestr}" $4 $1
		${If} $3 != "${flag}"
			; 拼接当前进程名
			StrCpy $2 "$2$3"
			; 最后一次循环，不需要Continue
			${If} $1 != $0
				${Continue}
			${EndIf}
		${EndIf}

		!insertmacro "NSDM_DEBUG_INFO" "ExeEnd->$2"
		; 取出来的是分隔符，表示$2中存储的已经是一个路径名了
		nsutil::NSFindProcess "$2" ""
		Pop $R0
		${If} $R0 <> 1
			; 当前进程不存在或者已被结束，寻找下一个进程
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
            nsutil::NSFindProcess "$2" ""
            Pop $R0
            ${If} $R0 == 1
	          StrCpy $5 -1
			  ${Break}
			${Else}
			  StrCpy $5 1
			${EndIf}
		${EndIf}
	${Next}
	Push $5
!macroend

; 检测exe进程，如有多个进程，以${flag}分隔
!macro ProcessDetect  exestr flag
; $0 总长
; $1 索引
; $2 目标字符串
; $3 临时比较变量
; $4 分隔符长度
; $5 返回值 1 找到进程; 0 没有找到进程;
	StrLen $0 "${exestr}"
	IntOp $1 0 + 0
	StrCpy $2 ""
	StrLen $4 "${flag}"
	StrCpy $5 0

	${ForEach} $1 0 $0 + 1
		StrCpy $3 "${exestr}" $4 $1
		${If} $3 != "${flag}"
			; 拼接当前进程名
			StrCpy $2 "$2$3"
			; 最后一次循环，不需要Continue
			${If} $1 != $0
				${Continue}
			${EndIf}
		${EndIf}

		!insertmacro "NSDM_DEBUG_INFO" "ProcessDetect->$2"
		; 取出来的是分隔符，表示$2中存储的已经是一个路径名了
		nsutil::NSFindProcess "$2" ""
		Pop $R0
		${If} $R0 <> 1
			; 当前进程不存在或者已被结束，寻找下一个进程
			StrCpy $2 ""
			${Continue}
		${Else}
			StrCpy $5 1
			${Break}
		${EndIf}
	${Next}
	Push $5
!macroend

; 刷新图标缓存
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


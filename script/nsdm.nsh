!ifndef __NSDM_NSH__
!define __NSDM_NSH__

#################################################################################
# FUNCTION DECLARTIONS
#################################################################################
!define nsdm.InitSkinEngine        '!insertmacro _NSDM_InitSkinEngine'
!define nsdm.FindControl           '!insertmacro _NSDM_FindControl'
!define nsdm.ShowLicense           '!insertmacro _NSDM_ShowLicense'
!define nsdm.RegisterCommandNotify '!insertmacro _NSDM_RegisterCommandNotify'
!define nsdm.SetControlAttr        '!insertmacro _NSDM_SetControlAttr'
!define nsdm.GetControlAttr        '!insertmacro _NSDM_GetControlAttr'
!define nsdm.CreateTimer           '!insertmacro _NSDM_CreateTimer'
!define nsdm.KillTimer             '!insertmacro _NSDM_KillTimer'
!define nsdm.SendMessage           '!insertmacro _NSDM_SendMessage'
!define nsdm.SelectFolderDialog    '!insertmacro _NSDM_SelectFolderDialog'
!define nsdm.Run                   '!insertmacro _NSDM_Run'
!define nsdm.UninitSkinEngine      '!insertmacro _NSDM_UninitSkinEngine'
!define nsdm.MessageBox            '!insertmacro _NSDM_MessageBox'
!define nsdm.VerifyCharaters       '!insertmacro _NSDM_VerifyCharaters'
!define nsdm.SelectPage            '!insertmacro _NSDM_SelectPage'
!define nsdm.PrevPage              '!insertmacro _NSDM_PrevPage'
!define nsdm.NextPage              '!insertmacro _NSDM_NextPage'
!define nsdm.GetWindowAttr         '!insertmacro _NSDM_GetWindowAttr'
!define nsdm.SetWindowAttr         '!insertmacro _NSDM_SetWindowAttr'


#################################################################################
# MACRO DEFINITIONS
#################################################################################

!macro _NSDM_InitSkinEngine          skin_path xml_id       ctl_name     out
       nsdm::NSInitSkinEngine        /NOUNLOAD ${skin_path} ${xml_id}    ${ctl_name}
       Pop                           ${out}
!macroend

!macro _NSDM_FindControl             ctl_name  out
       nsdm::NSFindControl           /NOUNLOAD ${ctl_name}
       Pop                           ${out}
!macroend

!macro _NSDM_ShowLicense             xml_id    title_id     title        license_id  license_path  out
       nsdm::NSShowLicense           /NOUNLOAD ${xml_id}    ${title_id}  ${title}    ${license_id} ${license_path}
       Pop                           ${out}
!macroend

!macro _NSDM_RegisterCommandNotify   func_name func_addr
       nsdm::NSRegisterCommandNotify /NOUNLOAD ${func_name} ${func_addr}
!macroend

!macro _NSDM_SetControlAttr          ctl_name  data         type
       nsdm::NSSetControlAttr        /NOUNLOAD ${ctl_name}  ${data}      ${type}
!macroend

!macro _NSDM_GetControlAttr          ctl_name  type         out
       nsdm::NSGetControlAttr        /NOUNLOAD ${ctl_name}  ${type}
       Pop                           ${out}
!macroend

!macro _NSDM_CreateTimer             id        interval
       nsdm::NSCreateTimer           /NOUNLOAD ${id}        ${interval}
!macroend

!macro _NSDM_KillTimer               id
       nsdm::NSKillTimer             /NOUNLOAD ${id}
!macroend

!macro _NSDM_SendMessage             msg       wp           lp
       nsdm::NSSendMessage           /NOUNLOAD ${msg}       ${wp}        ${lp}
!macroend

!macro _NSDM_SelectFolderDialog      title     out
       nsdm::NSSelectFolderDialog    /NOUNLOAD ${title}
       Pop                           ${out}
!macroend

!macro _NSDM_Run
       nsdm::NSRun
!macroend

!macro _NSDM_UninitSkinEngine
       nsdm::NSUninitSkinEngine      /NOUNLOAD
!macroend

!macro _NSDM_MessageBox              xml_id   size  title_id   title   content_id  content       type            out
       nsdm::NSMessageBox            /NOUNLOAD  ${xml_id} ${size} ${title_id} "${title}"    ${content_id} "${content}"      ${type}
       Pop                           ${out}
!macroend

!macro _NSDM_VerifyCharaters         string    out
       nsdm::NSVerifyCharaters       /NOUNLOAD ${string}
       Pop                           ${out}
!macroend

!macro _NSDM_SelectPage              page_name
       nsdm::NSSelectPage            /NOUNLOAD ${page_name}
!macroend

!macro _NSDM_SetWindowAttr           data      type
       nsdm::NSSetWindowAttr         /NOUNLOAD ${data}      ${type}
!macroend

!macro _NSDM_GetWindowAttr           type      out
       nsdm::NSGetWindowAttr         /NOUNLOAD ${type}
       Pop                           ${out}
!macroend

!macro _NSDM_PrevPage
       nsdm::NSPrevPage              /NOUNLOAD
!macroend

!macro _NSDM_NextPage
       nsdm::NSNextPage              /NOUNLOAD
!macroend

!endif # __NSDM_NSH__


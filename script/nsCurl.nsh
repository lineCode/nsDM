!ifndef __NSCURL_NSH__
!define __NSCURL_NSH__

#################################################################################
# FUNCTION DECLARTIONS
#################################################################################
!define nscurl.Get       '!insertmacro _NSCURL_Get'
!define nscurl.GetAsync  '!insertmacro _NSCURL_GetAsync'
!define nscurl.Post      '!insertmacro _NSCURL_Post'
!define nscurl.PostAsync '!insertmacro _NSCURL_PostAsync'
!define nscurl.WaitDone  '!insertmacro _NSCURL_WaitDone'

#################################################################################
# MACRO DEFINITIONS
#################################################################################
!macro _NSCURL_Get         url          out
       nscurl::NSGet       ${url}
       Pop                 ${out}
!macroend

!macro _NSCURL_GetAsync    url          out
       nscurl::NSGetAsync  /NOUNLOAD    ${url}
       Pop                 ${out}
!macroend

!macro _NSCURL_Post        url          data    out
       nscurl::NSPost      ${url}       ${data}
       Pop                 ${out}
!macroend

!macro _NSCURL_PostAsync   url          data    out
       nscurl::NSPostAsync /NOUNLOAD    ${url}  ${data}
       Pop                 ${out}
!macroend

!macro _NSCURL_WaitDone    timeout     out
	   nscurl::NSWaitDone  ${timeout}
	   Pop ${out}
!macroend

!endif # __NSCURL_NSH__
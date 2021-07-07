
!ifndef RegUninstallIcon_INCLUDED
!define RegUninstallIcon_INCLUDED
 
!include Util.nsh
 
!verbose push
!verbose 3
!ifndef _RegUninstallIcon_VERBOSE
  !define _RegUninstallIcon_VERBOSE 3
!endif
!verbose ${_RegUninstallIcon_VERBOSE}
!define RegUninstallIcon_VERBOSE `!insertmacro RegUninstallIcon_VERBOSE`
!verbose pop
 
!macro RegUninstallIcon_VERBOSE _VERBOSE
  !verbose push
  !verbose 3
  !undef _RegUninstallIcon_VERBOSE
  !define _RegUninstallIcon_VERBOSE ${_VERBOSE}
  !verbose pop
!macroend
 
 
 
!macro RegUninstallIconCall _KEY _ICONFILE
  !verbose push
  !verbose ${_RegUninstallIcon_VERBOSE}
  Push `${_KEY}`
  Push `${_ICONFILE}`
  ${CallArtificialFunction} RegUninstallIcon_
  !verbose pop
!macroend
 
 
!define RegUninstallIcon `!insertmacro RegUninstallIconCall`
 
!macro RegUninstallIcon
!macroend
  
!macro RegUninstallIcon_
  !verbose push
  !verbose ${_RegUninstallIcon_VERBOSE}
 
  Exch $R1 ;IconFile
  Exch
  Exch $R0 ;Key
  Exch
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$R0" "DisplayIcon" "$R1,0"

  Pop $R0
  Pop $R1
 
  !verbose pop
!macroend
 
!endif # !RegUninstallIcon_INCLUDED


;iQPuzzle NSIS installer script

  !define APPNAME "iQPuzzle"
  !define DESCRIPTION "A diverting I.Q. challenging pentomino puzzle"
  !define VERSION "1.1.2.0" ;use always 4 digits w.x.y.z
  
  !include "LogicLib.nsh"
  !include "MUI2.nsh"
  !include "FileFunc.nsh"
  
  Name "${APPNAME}"
  OutFile "${APPNAME}_Installer.exe"
  InstallDir "$PROGRAMFILES\${APPNAME}"
  !define MUI_ICON "iqpuzzle.ico"
  RequestExecutionLevel admin

  VIProductVersion ${VERSION}
  VIAddVersionKey ProductName "${APPNAME}"
  VIAddVersionKey LegalCopyright "Thorsten Roth"
  VIAddVersionKey FileDescription "${DESCRIPTION}"
  VIAddVersionKey FileVersion "${VERSION}"
  VIAddVersionKey ProductVersion "${VERSION}"
  VIAddVersionKey InternalName "${APPNAME}"
  BrandingText "${APPNAME} - ${VERSION}"
  
  Var StartMenuFolder
  !define MUI_ABORTWARNING
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_LANGUAGE "English"
  
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${APPNAME}" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

;--------------------------------
;Installer section

Section

  SetOutPath "$INSTDIR"
  
  ;Add all files from folder iQPuzzle into installer
  File /r ${APPNAME}\*.*
    
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\${APPNAME}.lnk" "$INSTDIR\iQPuzzle.exe"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END

  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" \
                     "EstimatedSize" "$0"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" \
                   "DisplayName" "${APPNAME}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" \
                   "Publisher" "Thorsten Roth"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" \
                   "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" \
                   "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""
  
SectionEnd
 
;--------------------------------
;Uninstaller section

Section "Uninstall"

  Delete "$INSTDIR\Uninstall.exe"
  RMDir /r "$INSTDIR"
  
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\${APPNAME}.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
  
SectionEnd

;--------------------------------

Function .onInit
  UserInfo::GetAccountType
  pop $0
  ${If} $0 != "admin" ;Require admin rights on NT4+
    MessageBox mb_iconstop "Administrator rights required for installation!"
    SetErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
    Quit
  ${EndIf}
FunctionEnd
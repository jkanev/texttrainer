
;______________________________________________________
;
;   I n t r o d u c t i o n
;______________________________________________________


Name "TextTrainer"

!define VERSION "0.1.0"

OutFile "TextTrainer-${VERSION}_Setup.exe"

InstallDir $PROGRAMFILES\TextTrainer
RequestExecutionLevel admin ; application privileges for Vista

!include Library.nsh


;______________________________________________________
;
;   P a g e s
;______________________________________________________


Page directory
Page instfiles
UninstPage instfiles


;______________________________________________________
;
;   S e c t i o n s
;______________________________________________________


;______________________________________________________
;   C o r e


Section "install.core"
	

	; install language files
	SetOutPath $INSTDIR
	File /r "..\i18n"

	; install main software
	SetOutPath $INSTDIR
        File /r "..\bin\*"
	
	; write registry entries
	WriteRegStr HKEY_LOCAL_MACHINE "Software\TextTrainer\TextTrainer" "i18n" "$INSTDIR\i18n\"
	WriteRegStr HKEY_LOCAL_MACHINE "Software\TextTrainer\TextTrainer" "LastText" "$INSTDIR\data\hoelderlin_gang-aufs-land.utf"
	WriteRegStr HKEY_CURRENT_USER "Software\TextTrainer\TextTrainer" "i18n" "$INSTDIR\i18n\"
	WriteRegStr HKEY_CURRENT_USER "Software\TextTrainer\TextTrainer" "LastText" "$INSTDIR\data\hoelderlin_gang-aufs-land.utf"
	
	; create uninstaller
	WriteUninstaller $INSTDIR\uninstall.exe
	
	; create menu shortcuts
	CreateDirectory "$SMPROGRAMS\TextTrainer"
	CreateShortCut "$SMPROGRAMS\TextTrainer\TextTrainer.lnk" "$INSTDIR\texttrainer.exe"
	CreateShortCut "$SMPROGRAMS\TextTrainer\Uninstall.lnk" "$INSTDIR\uninstall.exe"

SectionEnd


Section "un.install.core"
	
	; delete libraries
        !insertmacro UnInstallLib REGDLL SHARED REBOOT_NOTPROTECTED $SYSDIR\icudt53.dll
        !insertmacro UnInstallLib REGDLL SHARED REBOOT_NOTPROTECTED $SYSDIR\icuin53.dll
        !insertmacro UnInstallLib REGDLL SHARED REBOOT_NOTPROTECTED $SYSDIR\icuuc53.dll
        !insertmacro UnInstallLib REGDLL SHARED REBOOT_NOTPROTECTED $SYSDIR\libgcc_s_dw2-1.dll
        !insertmacro UnInstallLib REGDLL SHARED REBOOT_NOTPROTECTED $SYSDIR\libstdc++-6.dll
        !insertmacro UnInstallLib REGDLL SHARED REBOOT_NOTPROTECTED $SYSDIR\libwinpthread-1.dll
        !insertmacro UnInstallLib REGDLL SHARED REBOOT_NOTPROTECTED $SYSDIR\Qt5Core.dll
        !insertmacro UnInstallLib REGDLL SHARED REBOOT_NOTPROTECTED $SYSDIR\Qt5Gui.dll
        !insertmacro UnInstallLib REGDLL SHARED REBOOT_NOTPROTECTED $SYSDIR\Qt5Widgets.dll

	; delete registry entries
	DeleteRegKey HKEY_LOCAL_MACHINE "TextTrainer"
	DeleteRegKey HKEY_CURRENT_USER "TextTrainer"
	
	; delete uninstaller
	Delete $INSTDIR\TextTrainer_Uninstall.exe
	
	; delete menu shortcuts
	Delete "$SMPROGRAMS\TextTrainer\TextTrainer.lnk"
	Delete "$SMPROGRAMS\TextTrainer\Uninstall.lnk"
	RmDir "$SMPROGRAMS\TextTrainer"
	
	RmDir /r $INSTDIR
	
SectionEnd


;______________________________________________________
;   T e x t   F i l e s


Section "install.data"

	SetOutPath "$INSTDIR"
	File /r /x ".svn" "..\data"
	createShortCut "$SMPROGRAMS\TextTrainer\Texts.lnk" "$INSTDIR\data"

SectionEnd


Section "un.install.data"

	RmDir /r "$INSTDIR\data"
	delete "$SMPROGRAMS\TextTrainer\Texts.lnk" 

SectionEnd


;______________________________________________________
;   M a n u a l


Section "install.manual"

	SetOutPath "$INSTDIR"
	File /r /x ".svn" "..\manual"
	createShortCut "$SMPROGRAMS\TextTrainer\Manual.lnk" "$INSTDIR\manual\texttrainer.html"

SectionEnd


Section "un.install.manual"

	RmDir /r "$INSTDIR\manual"
	delete "$SMPROGRAMS\TextTrainer\Manual.lnk"

SectionEnd



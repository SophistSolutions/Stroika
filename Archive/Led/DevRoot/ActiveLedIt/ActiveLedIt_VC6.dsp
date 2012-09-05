# Microsoft Developer Studio Project File - Name="ActiveLedIt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ActiveLedIt - Win32 Partial UNICODE DEMO Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ActiveLedIt_VC6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ActiveLedIt_VC6.mak" CFG="ActiveLedIt - Win32 Partial UNICODE DEMO Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ActiveLedIt - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ActiveLedIt - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ActiveLedIt - Win32 Full UNICODE Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ActiveLedIt - Win32 Full UNICODE Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ActiveLedIt - Win32 DEMO Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ActiveLedIt - Win32 DEMO Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ActiveLedIt - Win32 Partial UNICODE Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ActiveLedIt - Win32 Partial UNICODE Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ActiveLedIt - Win32 Partial UNICODE DEMO Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ActiveLedIt - Win32 Partial UNICODE DEMO Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ActiveLedIt - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Ext "ocx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Ext "ocx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "Headers" /I "..\Led\Headers" /D "NDEBUG" /D "_USRDLL" /D "WIN32" /D "_WINDOWS" /D qSupportOLEControlEmbedding=0 /D "_WINDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "NDEBUG" /d qWindows=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Release\ActiveLedIt_VC6.ocx
InputPath=.\Release\ActiveLedIt_VC6.ocx
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ActiveLedIt - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Ext "ocx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Ext "ocx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_USRDLL" /D "_WINDLL" /D "_DEBUG" /D "qDebug" /D "qHeavyDebugging" /D "WIN32" /D "_WINDOWS" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "_DEBUG" /d qWindows=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\ActiveLedIt_VC6.ocx
InputPath=.\Debug\ActiveLedIt_VC6.ocx
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ActiveLedIt - Win32 Full UNICODE Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugU"
# PROP BASE Intermediate_Dir "DebugU"
# PROP BASE Target_Ext "ocx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU"
# PROP Intermediate_Dir "DebugU"
# PROP Target_Ext "ocx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_USRDLL" /D "_WINDLL" /D "_UNICODE" /D "_DEBUG" /D "qDebug" /D "qHeavyDebugging" /D "WIN32" /D "_WINDOWS" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "_DEBUG" /d qWindows=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\DebugU
TargetPath=.\DebugU\ActiveLedIt_VC6.ocx
InputPath=.\DebugU\ActiveLedIt_VC6.ocx
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ActiveLedIt - Win32 Full UNICODE Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseU"
# PROP BASE Intermediate_Dir "ReleaseU"
# PROP BASE Target_Ext "ocx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseU"
# PROP Intermediate_Dir "ReleaseU"
# PROP Target_Ext "ocx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "Headers" /I "..\Led\Headers" /D "_USRDLL" /D "_UNICODE" /D "_WINDLL" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "NDEBUG" /d qWindows=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\ReleaseU
TargetPath=.\ReleaseU\ActiveLedIt_VC6.ocx
InputPath=.\ReleaseU\ActiveLedIt_VC6.ocx
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ActiveLedIt - Win32 DEMO Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ActiveLedIt___Win32_DEMO_Debug"
# PROP BASE Intermediate_Dir "ActiveLedIt___Win32_DEMO_Debug"
# PROP BASE Target_Ext "ocx"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ActiveLedIt___Win32_DEMO_Debug"
# PROP Intermediate_Dir "ActiveLedIt___Win32_DEMO_Debug"
# PROP Target_Ext "ocx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "\Led\Headers" /D "_DEBUG" /D "_USRDLL" /D "WIN32" /D "_WINDOWS" /D qSupportOLEControlEmbedding=0 /D "_WINDLL" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_USRDLL" /D qDemoMode=1 /D "_WINDLL" /D "_DEBUG" /D "qDebug" /D "qHeavyDebugging" /D "WIN32" /D "_WINDOWS" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "Headers" /i "\Led\Headers" /d "_DEBUG"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "_DEBUG" /d qDemoMode=1 /d qWindows=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\ActiveLedIt___Win32_DEMO_Debug
TargetPath=.\ActiveLedIt___Win32_DEMO_Debug\ActiveLedIt_VC6.ocx
InputPath=.\ActiveLedIt___Win32_DEMO_Debug\ActiveLedIt_VC6.ocx
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ActiveLedIt - Win32 DEMO Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ActiveLedIt___Win32_DEMO_Release"
# PROP BASE Intermediate_Dir "ActiveLedIt___Win32_DEMO_Release"
# PROP BASE Target_Ext "ocx"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ActiveLedIt___Win32_DEMO_Release"
# PROP Intermediate_Dir "ActiveLedIt___Win32_DEMO_Release"
# PROP Target_Ext "ocx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I "Headers" /I "\Led\Headers" /D "NDEBUG" /D "_USRDLL" /D "WIN32" /D "_WINDOWS" /D qSupportOLEControlEmbedding=0 /D "_WINDLL" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "Headers" /I "..\Led\Headers" /D "NDEBUG" /D "_USRDLL" /D qDemoMode=1 /D "_WINDLL" /D "WIN32" /D "_WINDOWS" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "Headers" /i "\Led\Headers" /d "NDEBUG"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "NDEBUG" /d qDemoMode=1 /d qWindows=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\ActiveLedIt___Win32_DEMO_Release
TargetPath=.\ActiveLedIt___Win32_DEMO_Release\ActiveLedIt_VC6.ocx
InputPath=.\ActiveLedIt___Win32_DEMO_Release\ActiveLedIt_VC6.ocx
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ActiveLedIt - Win32 Partial UNICODE Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ActiveLedIt___Win32_Partial_UNICODE_Debug"
# PROP BASE Intermediate_Dir "ActiveLedIt___Win32_Partial_UNICODE_Debug"
# PROP BASE Target_Ext "ocx"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ActiveLedIt___Win32_Partial_UNICODE_Debug"
# PROP Intermediate_Dir "ActiveLedIt___Win32_Partial_UNICODE_Debug"
# PROP Target_Ext "ocx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_USRDLL" /D "_WINDLL" /D "_DEBUG" /D "qDebug" /D "qHeavyDebugging" /D "WIN32" /D "_WINDOWS" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_USRDLL" /D "_WINDLL" /D "_DEBUG" /D "qDebug" /D "qHeavyDebugging" /D "WIN32" /D "_WINDOWS" /D qSupportOLEControlEmbedding=0 /D qLed_CharacterSet=3 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "_DEBUG"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "_DEBUG" /d qWindows=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\ActiveLedIt___Win32_Partial_UNICODE_Debug
TargetPath=.\ActiveLedIt___Win32_Partial_UNICODE_Debug\ActiveLedIt_VC6.ocx
InputPath=.\ActiveLedIt___Win32_Partial_UNICODE_Debug\ActiveLedIt_VC6.ocx
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ActiveLedIt - Win32 Partial UNICODE Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ActiveLedIt___Win32_Partial_UNICODE_Release"
# PROP BASE Intermediate_Dir "ActiveLedIt___Win32_Partial_UNICODE_Release"
# PROP BASE Target_Ext "ocx"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ActiveLedIt___Win32_Partial_UNICODE_Release"
# PROP Intermediate_Dir "ActiveLedIt___Win32_Partial_UNICODE_Release"
# PROP Target_Ext "ocx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I "Headers" /I "..\Led\Headers" /D "NDEBUG" /D "_USRDLL" /D "WIN32" /D "_WINDOWS" /D qSupportOLEControlEmbedding=0 /D "_WINDLL" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "Headers" /I "..\Led\Headers" /D "NDEBUG" /D "_USRDLL" /D "_WINDLL" /D "WIN32" /D "_WINDOWS" /D qSupportOLEControlEmbedding=0 /D qLed_CharacterSet=3 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "NDEBUG"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "NDEBUG" /d qWindows=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\ActiveLedIt___Win32_Partial_UNICODE_Release
TargetPath=.\ActiveLedIt___Win32_Partial_UNICODE_Release\ActiveLedIt_VC6.ocx
InputPath=.\ActiveLedIt___Win32_Partial_UNICODE_Release\ActiveLedIt_VC6.ocx
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ActiveLedIt - Win32 Partial UNICODE DEMO Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ActiveLedIt___Win32_Partial_UNICODE_DEMO_Debug"
# PROP BASE Intermediate_Dir "ActiveLedIt___Win32_Partial_UNICODE_DEMO_Debug"
# PROP BASE Target_Ext "ocx"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ActiveLedIt___Win32_Partial_UNICODE_DEMO_Debug"
# PROP Intermediate_Dir "ActiveLedIt___Win32_Partial_UNICODE_DEMO_Debug"
# PROP Target_Ext "ocx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_USRDLL" /D qDemoMode=1 /D "_WINDLL" /D qUseGDIScrollbitsForScrolling=0 /D "_DEBUG" /D "qDebug" /D "qHeavyDebugging" /D "WIN32" /D "_WINDOWS" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_USRDLL" /D qDemoMode=1 /D "_WINDLL" /D "_DEBUG" /D "qDebug" /D "qHeavyDebugging" /D qLed_CharacterSet=3 /D "WIN32" /D "_WINDOWS" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "_DEBUG" /d qDemoMode=1
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "_DEBUG" /d qDemoMode=1 /d qWindows=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\ActiveLedIt___Win32_Partial_UNICODE_DEMO_Debug
TargetPath=.\ActiveLedIt___Win32_Partial_UNICODE_DEMO_Debug\ActiveLedIt_VC6.ocx
InputPath=.\ActiveLedIt___Win32_Partial_UNICODE_DEMO_Debug\ActiveLedIt_VC6.ocx
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ActiveLedIt - Win32 Partial UNICODE DEMO Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ActiveLedIt___Win32_Partial_UNICODE_DEMO_Release"
# PROP BASE Intermediate_Dir "ActiveLedIt___Win32_Partial_UNICODE_DEMO_Release"
# PROP BASE Target_Ext "ocx"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ActiveLedIt___Win32_Partial_UNICODE_DEMO_Release"
# PROP Intermediate_Dir "ActiveLedIt___Win32_Partial_UNICODE_DEMO_Release"
# PROP Target_Ext "ocx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I "Headers" /I "..\Led\Headers" /D "NDEBUG" /D "_USRDLL" /D qDemoMode=1 /D "_WINDLL" /D "WIN32" /D "_WINDOWS" /D qSupportOLEControlEmbedding=0 /D qUseGDIScrollbitsForScrolling=0 /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "Headers" /I "..\Led\Headers" /D "_USRDLL" /D qDemoMode=1 /D "_WINDLL" /D "NDEBUG" /D "qWideCharacters" /D "WIN32" /D "_WINDOWS" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "NDEBUG" /d qDemoMode=1
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "NDEBUG" /d qDemoMode=1 /d qWindows=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\ActiveLedIt___Win32_Partial_UNICODE_DEMO_Release
TargetPath=.\ActiveLedIt___Win32_Partial_UNICODE_DEMO_Release\ActiveLedIt_VC6.ocx
InputPath=.\ActiveLedIt___Win32_Partial_UNICODE_DEMO_Release\ActiveLedIt_VC6.ocx
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "ActiveLedIt - Win32 Release"
# Name "ActiveLedIt - Win32 Debug"
# Name "ActiveLedIt - Win32 Full UNICODE Debug"
# Name "ActiveLedIt - Win32 Full UNICODE Release"
# Name "ActiveLedIt - Win32 DEMO Debug"
# Name "ActiveLedIt - Win32 DEMO Release"
# Name "ActiveLedIt - Win32 Partial UNICODE Debug"
# Name "ActiveLedIt - Win32 Partial UNICODE Release"
# Name "ActiveLedIt - Win32 Partial UNICODE DEMO Debug"
# Name "ActiveLedIt - Win32 Partial UNICODE DEMO Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Sources\ActiveLedIt.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ActiveLedIt.def
# End Source File
# Begin Source File

SOURCE=.\Sources\ActiveLedIt.odl
# ADD MTL /I "Headers"
# End Source File
# Begin Source File

SOURCE=.\Sources\ActiveLedIt.rc
# End Source File
# Begin Source File

SOURCE=.\Sources\ActiveLedItControl.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ActiveLedItPpg.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\FontMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedItView.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Headers\ActiveLedIt.h
# End Source File
# Begin Source File

SOURCE=.\Headers\ActiveLedItConfig.h
# End Source File
# Begin Source File

SOURCE=.\Headers\ActiveLedItControl.h
# End Source File
# Begin Source File

SOURCE=.\Headers\ActiveLedItPpg.h
# End Source File
# Begin Source File

SOURCE=.\Headers\DispIDs.h
# End Source File
# Begin Source File

SOURCE=.\Headers\FontMenu.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedItView.h
# End Source File
# Begin Source File

SOURCE=.\Headers\Resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Sources\AboutBoxImage.bmp
# End Source File
# Begin Source File

SOURCE=.\Sources\ActiveLedIt.ico
# End Source File
# Begin Source File

SOURCE=.\Sources\ActiveLedItControl.bmp
# End Source File
# Begin Source File

SOURCE=.\Sources\UnknownEmbeddingIcon.bmp
# End Source File
# Begin Source File

SOURCE=.\Sources\UnsupportedPictFormat.bmp
# End Source File
# Begin Source File

SOURCE=.\Sources\URLEmbeddingIcon.bmp
# End Source File
# End Group
# Begin Group "Led Headers"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\Led\Headers\ChunkedArrayTextStore.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\CodePage.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\Command.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\FlavorPackage.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\HiddenText.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\Led_MFC.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\Led_MFC_WordProcessor.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\Led_Win32.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\LedConfig.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\LedGDI.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\LedHandySimple.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\LedOptionsSupport.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\LedStdDialogs.h
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\LedStdDialogs.r
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\LedSupport.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\LineBasedPartition.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\Marker.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\MarkerCover.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\MultiRowTextImager.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\PartitioningTextImager.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\SimpleTextStore.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\StandardStyledTextImager.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\StandardStyledTextInteractor.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\StyledTextEmbeddedObjects.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\StyledTextImager.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\StyledTextIO.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\StyledTextIO_HTML.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\StyledTextIO_LedNative.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\StyledTextIO_PlainText.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\StyledTextIO_RTF.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\TextBreaks.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\TextImager.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\TextInteractor.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\TextInteractorMixins.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\TextStore.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\WordProcessor.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\WordWrappedTextImager.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\WordWrappedTextInteractor.h
# End Source File
# End Group
# Begin Group "IntermediateFiles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ActiveLedIt___Win32_Partial_UNICODE_Debug\ActiveLedIt.tlb
# End Source File
# Begin Source File

SOURCE=.\Debug\ActiveLedIt.tlb
# End Source File
# Begin Source File

SOURCE=.\Release\ActiveLedIt.tlb
# End Source File
# Begin Source File

SOURCE=.\ReleaseU\ActiveLedIt.tlb
# End Source File
# End Group
# Begin Group "CabFile"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ActiveLedIt.inf
# End Source File
# Begin Source File

SOURCE=.\ActiveLedIt___Win32_Partial_UNICODE_DEMO_Debug\ActiveLedIt.tlb
# End Source File
# Begin Source File

SOURCE=.\BuildCAB.bat
# End Source File
# End Group
# Begin Group "Samples"

# PROP Default_Filter ""
# Begin Group "HTML-VBScript"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Samples\HTML-VBScript\PreloadingSample.html"
# End Source File
# Begin Source File

SOURCE=".\Samples\HTML-VBScript\SimpleAutomation.html"
# End Source File
# Begin Source File

SOURCE=".\Samples\HTML-VBScript\SimpleAutomationAndFileIO.html"
# End Source File
# Begin Source File

SOURCE=".\Samples\HTML-VBScript\SimpleAutomationAndRTFAccess.html"
# End Source File
# Begin Source File

SOURCE=".\Samples\HTML-VBScript\SimplestSample.html"
# End Source File
# Begin Source File

SOURCE=".\Samples\HTML-VBScript\TwoControls.html"
# End Source File
# End Group
# Begin Group "HTML-JavaScript"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Samples\HTML-JavaScript\SimpleAutomation.html"
# End Source File
# End Group
# End Group
# Begin Group "System Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\BASETSD.H"
# End Source File
# End Group
# End Target
# End Project

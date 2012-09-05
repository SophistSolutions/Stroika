# Microsoft Developer Studio Project File - Name="NetLedIt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=NetLedIt - Win32 Debug Demo
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NetLedIt_VC6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NetLedIt_VC6.mak" CFG="NetLedIt - Win32 Debug Demo"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NetLedIt - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NetLedIt - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NetLedIt - Win32 Release Demo" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NetLedIt - Win32 Debug Demo" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NetLedIt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GR /GX /O2 /I "Headers" /I "...\..\IntermediateFiles" /I "..\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Common" /I "..\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Include" /I "..\Led\Headers" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D _WIN32_WINNT=0x0400 /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release/NPNetLedIt.dll"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "NetLedIt - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "...\..\IntermediateFiles" /I "..\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Common" /I "..\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Include" /I "..\Led\Headers" /D "_DEBUG" /D "DEBUG" /D "_WINDOWS" /D "WIN32" /D _WIN32_WINNT=0x0400 /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/NPNetLedIt.dll"

!ELSEIF  "$(CFG)" == "NetLedIt - Win32 Release Demo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "NetLedIt___Win32_Release_Demo"
# PROP BASE Intermediate_Dir "NetLedIt___Win32_Release_Demo"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "NetLedIt___Win32_Release_Demo"
# PROP Intermediate_Dir "NetLedIt___Win32_Release_Demo"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I "." /I "..\Source" /I "Headers" /I "...\..\IntermediateFiles" /I "\Sources" /I "\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Common" /I "\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Include" /I "\Led\Headers" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /I "Headers" /I "...\..\IntermediateFiles" /I "..\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Common" /I "..\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Include" /I "..\Led\Headers" /D "NDEBUG" /D qDemoMode=1 /D "_WINDOWS" /D "WIN32" /D _WIN32_WINNT=0x0400 /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "Headers" /i "\Led\Headers" /d "NDEBUG"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "NDEBUG" /d qDemoMode=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release/NPNetLedIt.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"NetLedIt___Win32_Release_Demo/NPNetLedIt.dll"

!ELSEIF  "$(CFG)" == "NetLedIt - Win32 Debug Demo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "NetLedIt___Win32_Debug_Demo"
# PROP BASE Intermediate_Dir "NetLedIt___Win32_Debug_Demo"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "NetLedIt___Win32_Debug_Demo"
# PROP Intermediate_Dir "NetLedIt___Win32_Debug_Demo"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "...\..\IntermediateFiles" /I "\Sources" /I "\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Common" /I "\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Include" /I "\Led\Headers" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "DEBUG" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "...\..\IntermediateFiles" /I "..\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Common" /I "..\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Include" /I "..\Led\Headers" /D "DEBUG" /D "_DEBUG" /D qDemoMode=1 /D "_WINDOWS" /D "WIN32" /D _WIN32_WINNT=0x0400 /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "Headers" /i "\Led\Headers" /d "_DEBUG"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "_DEBUG" /d qDemoMode=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/NPNetLedIt.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"NetLedIt___Win32_Debug_Demo/NPNetLedIt.dll"

!ENDIF 

# Begin Target

# Name "NetLedIt - Win32 Release"
# Name "NetLedIt - Win32 Debug"
# Name "NetLedIt - Win32 Release Demo"
# Name "NetLedIt - Win32 Debug Demo"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Sources\LedItView.cpp
# End Source File
# Begin Source File

SOURCE=.\NetLedIt.def

!IF  "$(CFG)" == "NetLedIt - Win32 Release"

!ELSEIF  "$(CFG)" == "NetLedIt - Win32 Debug"

!ELSEIF  "$(CFG)" == "NetLedIt - Win32 Release Demo"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "NetLedIt - Win32 Debug Demo"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Sources\NetLedIt.java

!IF  "$(CFG)" == "NetLedIt - Win32 Release"

# Begin Custom Build
WkspDir=.
InputPath=.\Sources\NetLedIt.java

"\NetLedIt\IntermediateFiles\NetLedIt.class" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir "$(WkspDir)\IntermediateFiles" 
	cd "$(WkspDir)\IntermediateFiles" 
	set MAKE_MODE=dos 
	make -f ..\Makefile.javastuff all 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "NetLedIt - Win32 Debug"

# Begin Custom Build
WkspDir=.
InputPath=.\Sources\NetLedIt.java

"\NetLedIt\IntermediateFiles\NetLedIt.class" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir "$(WkspDir)\IntermediateFiles" 
	cd "$(WkspDir)\IntermediateFiles" 
	set MAKE_MODE=dos 
	make -f ..\Makefile.javastuff all 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "NetLedIt - Win32 Release Demo"

# PROP BASE Exclude_From_Build 1
# Begin Custom Build
WkspDir=.
InputPath=.\Sources\NetLedIt.java

"\NetLedIt\IntermediateFiles\NetLedIt.class" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir "$(WkspDir)\IntermediateFiles" 
	cd "$(WkspDir)\IntermediateFiles" 
	set MAKE_MODE=dos 
	make -f ..\Makefile.javastuff all 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "NetLedIt - Win32 Debug Demo"

# PROP BASE Exclude_From_Build 1
# Begin Custom Build
WkspDir=.
InputPath=.\Sources\NetLedIt.java

"\NetLedIt\IntermediateFiles\NetLedIt.class" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir "$(WkspDir)\IntermediateFiles" 
	cd "$(WkspDir)\IntermediateFiles" 
	set MAKE_MODE=dos 
	make -f ..\Makefile.javastuff all 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Sources\NetLedIt.r
# End Source File
# Begin Source File

SOURCE=.\Sources\NetLedIt.rc
# End Source File
# Begin Source File

SOURCE=.\Sources\NetLedItMain.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\stubs.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Headers\DemoPreComp.pch
# End Source File
# Begin Source File

SOURCE=.\Headers\LedItView.h
# End Source File
# Begin Source File

SOURCE=.\Headers\NetLedItConfig.h
# End Source File
# Begin Source File

SOURCE=.\Headers\PreComp.pch
# End Source File
# Begin Source File

SOURCE=.\Headers\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Headers\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Sources\NetLedIt.ico
# End Source File
# Begin Source File

SOURCE=.\Sources\UnsupportedPictFormat.bmp
# End Source File
# End Group
# Begin Group "NetscapeSDK"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Include\jri.h
# End Source File
# Begin Source File

SOURCE=..\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Include\jri_md.h
# End Source File
# Begin Source File

SOURCE=..\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Include\jritypes.h
# End Source File
# Begin Source File

SOURCE=..\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Include\npapi.h
# End Source File
# Begin Source File

SOURCE=..\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Include\npupp.h
# End Source File
# Begin Source File

SOURCE=..\AuxiliaryPackages\Win32NetscapePluginSDK\PluginSDK\Common\npwin.cpp
# End Source File
# End Group
# Begin Group "IntermediateFiles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\IntermediateFiles\java_lang_Object.h
# End Source File
# Begin Source File

SOURCE=.\IntermediateFiles\NetLedIt.c

!IF  "$(CFG)" == "NetLedIt - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "NetLedIt - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "NetLedIt - Win32 Release Demo"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "NetLedIt - Win32 Debug Demo"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IntermediateFiles\NetLedIt.h
# End Source File
# Begin Source File

SOURCE=.\IntermediateFiles\netscape_plugin_Plugin.c

!IF  "$(CFG)" == "NetLedIt - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "NetLedIt - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "NetLedIt - Win32 Release Demo"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "NetLedIt - Win32 Debug Demo"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IntermediateFiles\netscape_plugin_Plugin.h
# End Source File
# End Group
# Begin Group "System Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\BASETSD.H"
# End Source File
# End Group
# Begin Group "Led Headers"

# PROP Default_Filter ""
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
# Begin Group "Samples"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Samples\ScriptingSampleNetLedItPage.html
# End Source File
# End Group
# Begin Source File

SOURCE=.\Makefile.javastuff
# End Source File
# End Target
# End Project

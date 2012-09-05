# Microsoft Developer Studio Project File - Name="LedLineIt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=LedLineIt - Win32 Full UNICODE Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LedLineIt_VC6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LedLineIt_VC6.mak" CFG="LedLineIt - Win32 Full UNICODE Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LedLineIt - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "LedLineIt - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "LedLineIt - Win32 Partial UNICODE Debug" (based on "Win32 (x86) Application")
!MESSAGE "LedLineIt - Win32 Full UNICODE Debug" (based on "Win32 (x86) Application")
!MESSAGE "LedLineIt - Win32 Partial UNICODE Release" (based on "Win32 (x86) Application")
!MESSAGE "LedLineIt - Win32 Full UNICODE Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LedLineIt - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GR /GX /O1 /I "Headers" /I "..\Led\Headers" /D "NDEBUG" /D qDebug=0 /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /D "qSingleByteCharacters" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "NDEBUG" /d "qWindows"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "LedLineIt - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_MBCS" /D "_DEBUG" /D "qDebug" /D "qHeavyDebugging" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /D "qSingleByteCharacters" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "_DEBUG" /d "qWindows"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "LedLineIt - Win32 Partial UNICODE Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LedLineIt___Win32_Partial_UNICODE_Debug"
# PROP BASE Intermediate_Dir "LedLineIt___Win32_Partial_UNICODE_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "LedLineIt___Win32_Partial_UNICODE_Debug"
# PROP Intermediate_Dir "LedLineIt___Win32_Partial_UNICODE_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_DEBUG" /D "qDebug" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_MBCS" /D "qWideCharacters" /D "_DEBUG" /D "qDebug" /D "qHeavyDebugging" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "_DEBUG" /d "qWindows"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "_DEBUG" /d "qWindows"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 nafxcwd.lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "LedLineIt - Win32 Full UNICODE Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LedLineIt___Win32_Full_UNICODE_Debug"
# PROP BASE Intermediate_Dir "LedLineIt___Win32_Full_UNICODE_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "LedLineIt___Win32_Full_UNICODE_Debug"
# PROP Intermediate_Dir "LedLineIt___Win32_Full_UNICODE_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_DEBUG" /D "qDebug" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_UNICODE" /D "_DEBUG" /D "qDebug" /D "qHeavyDebugging" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "_DEBUG" /d "qWindows"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "_DEBUG" /d "qWindows"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 nafxcwd.lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "LedLineIt - Win32 Partial UNICODE Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LedLineIt___Win32_Partial_UNICODE_Release"
# PROP BASE Intermediate_Dir "LedLineIt___Win32_Partial_UNICODE_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "LedLineIt___Win32_Partial_UNICODE_Release"
# PROP Intermediate_Dir "LedLineIt___Win32_Partial_UNICODE_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O1 /I "Headers" /I "..\Led\Headers" /D "NDEBUG" /D qDebug=0 /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /GR /GX /O1 /I "Headers" /I "..\Led\Headers" /D "NDEBUG" /D qDebug=0 /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /D "qWideCharacters" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "NDEBUG" /d "qWindows"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "NDEBUG" /d "qWindows"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 nafxcw.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "LedLineIt - Win32 Full UNICODE Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LedLineIt___Win32_Full_UNICODE_Release"
# PROP BASE Intermediate_Dir "LedLineIt___Win32_Full_UNICODE_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "LedLineIt___Win32_Full_UNICODE_Release"
# PROP Intermediate_Dir "LedLineIt___Win32_Full_UNICODE_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O1 /I "Headers" /I "..\Led\Headers" /D "NDEBUG" /D qDebug=0 /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /GR /GX /O1 /I "Headers" /I "..\Led\Headers" /D "NDEBUG" /D qDebug=0 /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "NDEBUG" /d "qWindows"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "NDEBUG" /d "qWindows"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 nafxcw.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "LedLineIt - Win32 Release"
# Name "LedLineIt - Win32 Debug"
# Name "LedLineIt - Win32 Partial UNICODE Debug"
# Name "LedLineIt - Win32 Full UNICODE Debug"
# Name "LedLineIt - Win32 Partial UNICODE Release"
# Name "LedLineIt - Win32 Full UNICODE Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Sources\FontMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedLineIt.odl
# End Source File
# Begin Source File

SOURCE=.\Sources\LedLineIt.rc

!IF  "$(CFG)" == "LedLineIt - Win32 Release"

# ADD BASE RSC /l 0x409 /i "Sources" /i ".\Release" /i ".\Debug"
# ADD RSC /l 0x409 /i "Sources" /i ".\Release" /i ".\Debug" /i ".\Sources" /i "$(OUTDIR)"

!ELSEIF  "$(CFG)" == "LedLineIt - Win32 Debug"

# ADD BASE RSC /l 0x409 /i "Sources" /i ".\Debug" /i ".\Release"
# ADD RSC /l 0x409 /i "Sources" /i ".\Debug" /i ".\Sources" /i "$(OUTDIR)" /i ".\Release"

!ELSEIF  "$(CFG)" == "LedLineIt - Win32 Partial UNICODE Debug"

# ADD BASE RSC /l 0x409 /i "Sources" /i "LedLineIt___Win32_Partial_UNICODE_Debug" /i ".\Debug" /i ".\Sources" /i "$(OUTDIR)" /i ".\Release"
# ADD RSC /l 0x409 /i "Sources" /i "LedLineIt___Win32_Partial_UNICODE_Debug" /i ".\Debug" /i ".\Sources" /i "$(OUTDIR)" /i ".\Release"

!ELSEIF  "$(CFG)" == "LedLineIt - Win32 Full UNICODE Debug"

# ADD BASE RSC /l 0x409 /i "Sources" /i "LedLineIt___Win32_Full_UNICODE_Debug" /i ".\Debug" /i ".\Sources" /i "$(OUTDIR)" /i ".\Release"
# ADD RSC /l 0x409 /i "Sources" /i "LedLineIt___Win32_Full_UNICODE_Debug" /i ".\Debug" /i ".\Sources" /i "$(OUTDIR)" /i ".\Release"

!ELSEIF  "$(CFG)" == "LedLineIt - Win32 Partial UNICODE Release"

# ADD BASE RSC /l 0x409 /i "Sources" /i "LedLineIt___Win32_Partial_UNICODE_Release" /i ".\Release" /i ".\Debug" /i ".\Sources" /i "$(OUTDIR)"
# ADD RSC /l 0x409 /i "Sources" /i "LedLineIt___Win32_Partial_UNICODE_Release" /i ".\Release" /i ".\Debug" /i ".\Sources" /i "$(OUTDIR)"

!ELSEIF  "$(CFG)" == "LedLineIt - Win32 Full UNICODE Release"

# ADD BASE RSC /l 0x409 /i "Sources" /i "LedLineIt___Win32_Full_UNICODE_Release" /i ".\Release" /i ".\Debug" /i ".\Sources" /i "$(OUTDIR)"
# ADD RSC /l 0x409 /i "Sources" /i "LedLineIt___Win32_Full_UNICODE_Release" /i ".\Release" /i ".\Debug" /i ".\Sources" /i "$(OUTDIR)"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Sources\LedLineItApplication.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedLineItDocFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedLineItDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedLineItInPlaceFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedLineItMainFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedLineItServerItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedLineItView.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\Options.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Headers\FontMenu.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedLineItApplication.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedLineItConfig.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedLineItDocFrame.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedLineItDocument.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedLineItInPlaceFrame.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedLineItMainFrame.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedLineItServerItem.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedLineItView.h
# End Source File
# Begin Source File

SOURCE=.\Headers\Options.h
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

SOURCE=.\Sources\IToolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedLineIt.ico
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

SOURCE=..\Led\Headers\Led_Win32.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\Led_Win32_FileRegistration.h
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

SOURCE=..\Led\Headers\LedStdDialogs.h
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

SOURCE=..\Led\Headers\SimpleTextImager.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\SimpleTextInteractor.h
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

SOURCE=..\Led\Headers\SyntaxColoring.h
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
# Begin Group "System Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\BASETSD.H"
# End Source File
# End Group
# End Target
# End Project

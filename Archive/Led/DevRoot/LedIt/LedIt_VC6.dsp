# Microsoft Developer Studio Project File - Name="LedIt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=LedIt - Win32 Full UNICODE Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LedIt_VC6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LedIt_VC6.mak" CFG="LedIt - Win32 Full UNICODE Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LedIt - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "LedIt - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "LedIt - Win32 Partial UNICODE Debug" (based on "Win32 (x86) Application")
!MESSAGE "LedIt - Win32 Full UNICODE Debug" (based on "Win32 (x86) Application")
!MESSAGE "LedIt - Win32 Partial UNICODE Release" (based on "Win32 (x86) Application")
!MESSAGE "LedIt - Win32 Full UNICODE Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LedIt - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GR /GX /O1 /I "Headers" /I "..\Led\Headers" /I "..\AuxiliaryPackages\Quicktime for Windows\Develop\Include" /D "NDEBUG" /D qDebug=0 /D "_MBCS" /D "qUseQuicktimeForWindows" /D "WIN32" /D "_WINDOWS" /D "qSingleByteCharacters" /FD /c
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
# ADD LINK32 "..\AuxiliaryPackages\Quicktime for Windows\Develop\MSVC32\qtim32.lib" /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "LedIt - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /I "..\AuxiliaryPackages\Quicktime for Windows\Develop\Include" /D "_MBCS" /D "qUseQuicktimeForWindows" /D "_DEBUG" /D "qDebug" /D "qHeavyDebugging" /D "WIN32" /D "_WINDOWS" /D "qSingleByteCharacters" /FD /c
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
# ADD LINK32 "..\AuxiliaryPackages\Quicktime for Windows\Develop\MSVC32\qtim32.lib" /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "LedIt - Win32 Partial UNICODE Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LedIt___Win32_Partial_UNICODE_Debug"
# PROP BASE Intermediate_Dir "LedIt___Win32_Partial_UNICODE_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "LedIt___Win32_Partial_UNICODE_Debug"
# PROP Intermediate_Dir "LedIt___Win32_Partial_UNICODE_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /I "..\AuxiliaryPackages\Quicktime for Windows\Develop\Include" /D "_DEBUG" /D "qDebug" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D "qUseQuicktimeForWindows" /D "qExtraWordWrapDebugging" /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /I "..\AuxiliaryPackages\Quicktime for Windows\Develop\Include" /D "_MBCS" /D "qUseQuicktimeForWindows" /D "qWideCharacters" /D "_DEBUG" /D "qDebug" /D "qHeavyDebugging" /D "WIN32" /D "_WINDOWS" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "_DEBUG" /d "qWindows"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "_DEBUG" /d "qWindows"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 "..\AuxiliaryPackages\Quicktime for Windows\Develop\MSVC32\qtim32.lib" /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 "..\AuxiliaryPackages\Quicktime for Windows\Develop\MSVC32\qtim32.lib" /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "LedIt - Win32 Full UNICODE Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LedIt___Win32_Full_UNICODE_Debug"
# PROP BASE Intermediate_Dir "LedIt___Win32_Full_UNICODE_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "LedIt___Win32_Full_UNICODE_Debug"
# PROP Intermediate_Dir "LedIt___Win32_Full_UNICODE_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /I "..\AuxiliaryPackages\Quicktime for Windows\Develop\Include" /D "_DEBUG" /D "qDebug" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D "qUseQuicktimeForWindows" /D "qExtraWordWrapDebugging" /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_UNICODE" /D "_DEBUG" /D "qDebug" /D "qHeavyDebugging" /D "WIN32" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "_DEBUG" /d "qWindows"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "_DEBUG" /d "qWindows"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 "..\AuxiliaryPackages\Quicktime for Windows\Develop\MSVC32\qtim32.lib" /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "LedIt - Win32 Partial UNICODE Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LedIt___Win32_Partial_UNICODE_Release"
# PROP BASE Intermediate_Dir "LedIt___Win32_Partial_UNICODE_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "LedIt___Win32_Partial_UNICODE_Release"
# PROP Intermediate_Dir "LedIt___Win32_Partial_UNICODE_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O1 /I "Headers" /I "..\Led\Headers" /I "..\AuxiliaryPackages\Quicktime for Windows\Develop\Include" /D "NDEBUG" /D qDebug=0 /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D "qUseQuicktimeForWindows" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /GR /GX /O1 /I "Headers" /I "..\Led\Headers" /I "..\AuxiliaryPackages\Quicktime for Windows\Develop\Include" /D "NDEBUG" /D qDebug=0 /D "_MBCS" /D "qUseQuicktimeForWindows" /D "qWideCharacters" /D "WIN32" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "NDEBUG" /d "qWindows"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "NDEBUG" /d "qWindows"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 "..\AuxiliaryPackages\Quicktime for Windows\Develop\MSVC32\qtim32.lib" /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 "..\AuxiliaryPackages\Quicktime for Windows\Develop\MSVC32\qtim32.lib" /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "LedIt - Win32 Full UNICODE Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LedIt___Win32_Full_UNICODE_Release"
# PROP BASE Intermediate_Dir "LedIt___Win32_Full_UNICODE_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "LedIt___Win32_Full_UNICODE_Release"
# PROP Intermediate_Dir "LedIt___Win32_Full_UNICODE_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O1 /I "Headers" /I "..\Led\Headers" /I "..\AuxiliaryPackages\Quicktime for Windows\Develop\Include" /D "NDEBUG" /D qDebug=0 /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D "qUseQuicktimeForWindows" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /GR /GX /O1 /I "Headers" /I "..\Led\Headers" /D "NDEBUG" /D qDebug=0 /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "NDEBUG" /d "qWindows"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /i "..\Led\Sources" /d "NDEBUG" /d "qWindows"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 "..\AuxiliaryPackages\Quicktime for Windows\Develop\MSVC32\qtim32.lib" /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "LedIt - Win32 Release"
# Name "LedIt - Win32 Debug"
# Name "LedIt - Win32 Partial UNICODE Debug"
# Name "LedIt - Win32 Full UNICODE Debug"
# Name "LedIt - Win32 Partial UNICODE Release"
# Name "LedIt - Win32 Full UNICODE Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Sources\ColorMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\FilteredFilePicker.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Sources\FormatToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedIt.odl
# End Source File
# Begin Source File

SOURCE=.\Sources\LedIt.r
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Sources\LedIt.rc
# ADD BASE RSC /l 0x409 /i "Sources" /i "LedIt___Win32_Full_UNICODE_Release" /i ".\Debug" /i ".\Release"
# ADD RSC /l 0x409 /i "Sources" /i "LedIt___Win32_Full_UNICODE_Release" /i ".\Debug" /i ".\Sources" /i "$(OUTDIR)" /i ".\Release" /d "WIN32"
# End Source File
# Begin Source File

SOURCE=.\Sources\LedItApplication.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedItControlItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedItDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedItInPlaceFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedItMainFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedItServerItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedItView.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\Main.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Sources\Options.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\RulerToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\Units.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Headers\ColorMenu.h
# End Source File
# Begin Source File

SOURCE=.\Headers\FilteredFilePicker.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Headers\FormatToolbar.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedItApplication.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedItConfig.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedItControlItem.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedItDocument.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedItInPlaceFrame.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedItMainFrame.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedItResources.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedItServerItem.h
# End Source File
# Begin Source File

SOURCE=.\Headers\LedItView.h
# End Source File
# Begin Source File

SOURCE=.\Headers\Options.h
# End Source File
# Begin Source File

SOURCE=.\Headers\PreComp.pch
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Headers\RulerToolbar.h
# End Source File
# Begin Source File

SOURCE=.\Headers\Units.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Sources\AboutBoxImage.bmp
# End Source File
# Begin Source File

SOURCE=.\Sources\Font.bmp
# End Source File
# Begin Source File

SOURCE=.\Sources\FormatBa.bmp
# End Source File
# Begin Source File

SOURCE=.\Sources\IToolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedIt.ico
# End Source File
# Begin Source File

SOURCE=.\Sources\LedItDocument.ico
# End Source File
# Begin Source File

SOURCE=.\Sources\LedItRTFDocument.ico
# End Source File
# Begin Source File

SOURCE=.\Sources\RulerBl.bmp
# End Source File
# Begin Source File

SOURCE=.\Sources\RulerBlM.bmp
# End Source File
# Begin Source File

SOURCE=.\Sources\RulerDo.bmp
# End Source File
# Begin Source File

SOURCE=.\Sources\RulerDoM.bmp
# End Source File
# Begin Source File

SOURCE=.\Sources\RulerTa.bmp
# End Source File
# Begin Source File

SOURCE=.\Sources\RulerTaM.bmp
# End Source File
# Begin Source File

SOURCE=.\Sources\RulerUp.bmp
# End Source File
# Begin Source File

SOURCE=.\Sources\RulerUpM.bmp
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

SOURCE=..\Led\Headers\BiDiLayoutEngine.h
# End Source File
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

SOURCE=..\Led\Headers\SimpleTextStore.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\SpellCheckEngine.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\SpellCheckEngine_Basic.h
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
# Begin Group "SystemHeaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\BASETSD.H"
# End Source File
# End Group
# Begin Source File

SOURCE=.\Makefile
# End Source File
# End Target
# End Project

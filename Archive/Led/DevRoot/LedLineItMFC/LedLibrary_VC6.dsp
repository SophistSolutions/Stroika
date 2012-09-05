# Microsoft Developer Studio Project File - Name="LedLibrary" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LedLibrary - Win32 Full UNICODE Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LedLibrary_VC6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LedLibrary_VC6.mak" CFG="LedLibrary - Win32 Full UNICODE Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LedLibrary - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LedLibrary - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "LedLibrary - Win32 Partial UNICODE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "LedLibrary - Win32 Full UNICODE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "LedLibrary - Win32 Partial UNICODE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LedLibrary - Win32 Full UNICODE Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LedLibrary - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\LedLibrary\Release"
# PROP BASE Intermediate_Dir ".\LedLibrary\Release"
# PROP BASE Target_Dir ".\LedLibrary"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\LedLibrary\Release"
# PROP Intermediate_Dir ".\LedLibrary\Release"
# PROP Target_Dir ".\LedLibrary"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GR /GX /O1 /I "..\Led\Headers" /D "NDEBUG" /D qDebug=0 /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /D "qSingleByteCharacters" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LedLibrary - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\LedLibrary\Debug"
# PROP BASE Intermediate_Dir ".\LedLibrary\Debug"
# PROP BASE Target_Dir ".\LedLibrary"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\LedLibrary\Debug"
# PROP Intermediate_Dir ".\LedLibrary\Debug"
# PROP Target_Dir ".\LedLibrary"
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /GR /GX /Z7 /Od /I "..\Led\Headers" /D "_MBCS" /D "_DEBUG" /D "qDebug" /D "qHeavyDebugging" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /D "qSingleByteCharacters" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LedLibrary - Win32 Partial UNICODE Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LedLibrary\Partial UNICODE Debug"
# PROP BASE Intermediate_Dir "LedLibrary\Partial UNICODE Debug"
# PROP BASE Target_Dir "LedLibrary"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "LedLibrary\Partial UNICODE Debug"
# PROP Intermediate_Dir "LedLibrary\Partial UNICODE Debug"
# PROP Target_Dir "LedLibrary"
# ADD BASE CPP /nologo /MTd /W3 /GR /GX /Z7 /Od /I "..\Led\Headers" /D "_DEBUG" /D "qDebug" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /GR /GX /Z7 /Od /I "..\Led\Headers" /D "_MBCS" /D "qWideCharacters" /D "_DEBUG" /D "qDebug" /D "qHeavyDebugging" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LedLibrary - Win32 Full UNICODE Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LedLibrary\Full UNICODE Debug"
# PROP BASE Intermediate_Dir "LedLibrary\Full UNICODE Debug"
# PROP BASE Target_Dir "LedLibrary"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "LedLibrary\Full UNICODE Debug"
# PROP Intermediate_Dir "LedLibrary\Full UNICODE Debug"
# PROP Target_Dir "LedLibrary"
# ADD BASE CPP /nologo /MTd /W3 /GR /GX /Z7 /Od /I "..\Led\Headers" /D "_DEBUG" /D "qDebug" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /GR /GX /Z7 /Od /I "..\Led\Headers" /D "_UNICODE" /D "_DEBUG" /D "qDebug" /D "qHeavyDebugging" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LedLibrary - Win32 Partial UNICODE Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LedLibrary\Partial UNICODE Release"
# PROP BASE Intermediate_Dir "LedLibrary\Partial UNICODE Release"
# PROP BASE Target_Dir "LedLibrary"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "LedLibrary\Partial UNICODE Release"
# PROP Intermediate_Dir "LedLibrary\Partial UNICODE Release"
# PROP Target_Dir "LedLibrary"
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O1 /I "..\Led\Headers" /D "NDEBUG" /D qDebug=0 /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /GR /GX /O1 /I "..\Led\Headers" /D "NDEBUG" /D qDebug=0 /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /D "qWideCharacters" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LedLibrary - Win32 Full UNICODE Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LedLibrary\Full UNICODE Release"
# PROP BASE Intermediate_Dir "LedLibrary\Full UNICODE Release"
# PROP BASE Target_Dir "LedLibrary"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "LedLibrary\Full UNICODE Release"
# PROP Intermediate_Dir "LedLibrary\Full UNICODE Release"
# PROP Target_Dir "LedLibrary"
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O1 /I "..\Led\Headers" /D "NDEBUG" /D qDebug=0 /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /GR /GX /O1 /I "..\Led\Headers" /D "NDEBUG" /D qDebug=0 /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "qWindows" /D qSupportOLEControlEmbedding=0 /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "LedLibrary - Win32 Release"
# Name "LedLibrary - Win32 Debug"
# Name "LedLibrary - Win32 Partial UNICODE Debug"
# Name "LedLibrary - Win32 Full UNICODE Debug"
# Name "LedLibrary - Win32 Partial UNICODE Release"
# Name "LedLibrary - Win32 Full UNICODE Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\Led\Sources\BiDiLayoutEngine.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\ChunkedArrayTextStore.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\CodePage.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\Command.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\FlavorPackage.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\IdleManager.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\Led_MFC.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\Led_Win32.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\Led_Win32_FileRegistration.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\LedGDI.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\LedStdDialogs.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\LedSupport.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\LineBasedPartition.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\Marker.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\PartitioningTextImager.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\SimpleTextImager.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\SimpleTextInteractor.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\SimpleTextStore.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\StyledTextImager.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\SyntaxColoring.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\TextBreaks.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\TextImager.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\TextInteractor.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\TextInteractorMixins.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\TextStore.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
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
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "System Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\BASETSD.H"
# End Source File
# End Group
# End Target
# End Project

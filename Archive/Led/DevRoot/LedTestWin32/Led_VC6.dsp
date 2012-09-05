# Microsoft Developer Studio Project File - Name="Led" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Led - Win32 Full UNICODE Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Led_VC6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Led_VC6.mak" CFG="Led - Win32 Full UNICODE Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Led - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Led - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Led - Win32 Partial UNICODE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Led - Win32 Full UNICODE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Led - Win32 Partial UNICODE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Led - Win32 Full UNICODE Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Led - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /I "..\Led\Headers" /D "_LIB" /D "NDEBUG" /D qDebug=0 /D "_MBCS" /D "WIN32" /D "STRICT" /D _WIN32_WINNT=0x0400 /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Led - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "..\Led\Headers" /D "_LIB" /D "_DEBUG" /D qDebug=1 /D "_MBCS" /D "WIN32" /D "STRICT" /D _WIN32_WINNT=0x0400 /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Led - Win32 Partial UNICODE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Led___Win32_Partial_UNICODE_Debug"
# PROP BASE Intermediate_Dir "Led___Win32_Partial_UNICODE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Led___Win32_Partial_UNICODE_Debug"
# PROP Intermediate_Dir "Led___Win32_Partial_UNICODE_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "..\Led\Headers" /D "_LIB" /D "_DEBUG" /D qDebug=1 /D "WIN32" /D "_MBCS" /D "STRICT" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "..\Led\Headers" /D "_LIB" /D "_DEBUG" /D qDebug=1 /D "_MBCS" /D "qWideCharacters" /D "WIN32" /D "STRICT" /D _WIN32_WINNT=0x0400 /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "_DEBUG"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Led - Win32 Full UNICODE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Led___Win32_Full_UNICODE_Debug"
# PROP BASE Intermediate_Dir "Led___Win32_Full_UNICODE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Led___Win32_Full_UNICODE_Debug"
# PROP Intermediate_Dir "Led___Win32_Full_UNICODE_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "..\Led\Headers" /D "_LIB" /D "_DEBUG" /D qDebug=1 /D "WIN32" /D "_MBCS" /D "STRICT" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "..\Led\Headers" /D "_LIB" /D "_DEBUG" /D qDebug=1 /D "_UNICODE" /D "WIN32" /D "STRICT" /D _WIN32_WINNT=0x0400 /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "_DEBUG"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Led - Win32 Partial UNICODE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Led___Win32_Partial_UNICODE_Release"
# PROP BASE Intermediate_Dir "Led___Win32_Partial_UNICODE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Led___Win32_Partial_UNICODE_Release"
# PROP Intermediate_Dir "Led___Win32_Partial_UNICODE_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GR /GX /O2 /I "..\Led\Headers" /D "_LIB" /D "NDEBUG" /D qDebug=0 /D "WIN32" /D "_MBCS" /D "STRICT" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /GR /GX /O2 /I "..\Led\Headers" /D "_LIB" /D "NDEBUG" /D qDebug=0 /D "_MBCS" /D "qWideCharacters" /D "WIN32" /D "STRICT" /D _WIN32_WINNT=0x0400 /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "NDEBUG"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Led - Win32 Full UNICODE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Led___Win32_Full_UNICODE_Release"
# PROP BASE Intermediate_Dir "Led___Win32_Full_UNICODE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Led___Win32_Full_UNICODE_Release"
# PROP Intermediate_Dir "Led___Win32_Full_UNICODE_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GR /GX /O2 /I "..\Led\Headers" /D "_LIB" /D "NDEBUG" /D qDebug=0 /D "WIN32" /D "_MBCS" /D "STRICT" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /GR /GX /O2 /I "..\Led\Headers" /D "_LIB" /D "NDEBUG" /D qDebug=0 /D "_UNICODE" /D "WIN32" /D "STRICT" /D _WIN32_WINNT=0x0400 /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "NDEBUG"
# ADD RSC /l 0x409 /i "Headers" /i "..\Led\Headers" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Led - Win32 Release"
# Name "Led - Win32 Debug"
# Name "Led - Win32 Partial UNICODE Debug"
# Name "Led - Win32 Full UNICODE Debug"
# Name "Led - Win32 Partial UNICODE Release"
# Name "Led - Win32 Full UNICODE Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
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

SOURCE=..\Led\Sources\HiddenText.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\IdleManager.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\Led_Win32.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\LedGDI.cpp
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

SOURCE=..\Led\Sources\MarkerCover.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\MultiRowTextImager.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\PartitioningTextImager.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\SimpleLed.cpp
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

SOURCE=..\Led\Sources\StandardStyledTextImager.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\StandardStyledTextInteractor.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\StyledTextEmbeddedObjects.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\StyledTextImager.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\StyledTextIO.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\StyledTextIO_HTML.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\StyledTextIO_LedNative.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\StyledTextIO_MIMETextEnriched.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\StyledTextIO_PlainText.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\StyledTextIO_RTF.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\StyledTextIO_STYLText.cpp
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
# Begin Source File

SOURCE=..\Led\Sources\WordProcessor.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\WordWrappedTextImager.cpp
# End Source File
# Begin Source File

SOURCE=..\Led\Sources\WordWrappedTextInteractor.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
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

SOURCE=..\Led\Headers\SimpleLed.h
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

SOURCE=..\Led\Headers\StyledTextIO_HTML.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\StyledTextIO_LedNative.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\StyledTextIO_MIMETextEnriched.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\StyledTextIO_PlainText.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\StyledTextIO_RTF.h
# End Source File
# Begin Source File

SOURCE=..\Led\Headers\StyledTextIO_STYLText.h
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

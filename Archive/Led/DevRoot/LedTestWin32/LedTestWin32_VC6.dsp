# Microsoft Developer Studio Project File - Name="LedTestWin32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=LedTestWin32 - Win32 Full UNICODE Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LedTestWin32_VC6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LedTestWin32_VC6.mak" CFG="LedTestWin32 - Win32 Full UNICODE Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LedTestWin32 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "LedTestWin32 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "LedTestWin32 - Win32 Partial UNICODE Debug" (based on "Win32 (x86) Application")
!MESSAGE "LedTestWin32 - Win32 Full UNICODE Debug" (based on "Win32 (x86) Application")
!MESSAGE "LedTestWin32 - Win32 Partial UNICODE Release" (based on "Win32 (x86) Application")
!MESSAGE "LedTestWin32 - Win32 Full UNICODE Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LedTestWin32 - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /I "Headers" /I "..\Led\Headers" /D "_WINDOWS" /D "NDEBUG" /D qDebug=0 /D "_MBCS" /D "WIN32" /D "STRICT" /D _WIN32_WINNT=0x0400 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "\Led\Headers" /i "Headers" /i "..\Led\Headers" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "LedTestWin32 - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_WINDOWS" /D "_DEBUG" /D qDebug=1 /D "_MBCS" /D "WIN32" /D "STRICT" /D _WIN32_WINNT=0x0400 /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "\Led\Headers" /i "Headers" /i "..\Led\Headers" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "LedTestWin32 - Win32 Partial UNICODE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LedTestWin32___Win32_Partial_UNICODE_Debug"
# PROP BASE Intermediate_Dir "LedTestWin32___Win32_Partial_UNICODE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "LedTestWin32___Win32_Partial_UNICODE_Debug"
# PROP Intermediate_Dir "LedTestWin32___Win32_Partial_UNICODE_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_WINDOWS" /D "_DEBUG" /D qDebug=1 /D "WIN32" /D "_MBCS" /D "STRICT" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_WINDOWS" /D "_DEBUG" /D qDebug=1 /D "_MBCS" /D "qWideCharacters" /D "WIN32" /D "STRICT" /D _WIN32_WINNT=0x0400 /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "\Led\Headers" /i "Headers" /i "..\Led\Headers" /d "_DEBUG"
# ADD RSC /l 0x409 /i "\Led\Headers" /i "Headers" /i "..\Led\Headers" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "LedTestWin32 - Win32 Full UNICODE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LedTestWin32___Win32_Full_UNICODE_Debug"
# PROP BASE Intermediate_Dir "LedTestWin32___Win32_Full_UNICODE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "LedTestWin32___Win32_Full_UNICODE_Debug"
# PROP Intermediate_Dir "LedTestWin32___Win32_Full_UNICODE_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_WINDOWS" /D "_DEBUG" /D qDebug=1 /D "WIN32" /D "_MBCS" /D "STRICT" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "Headers" /I "..\Led\Headers" /D "_WINDOWS" /D "_DEBUG" /D qDebug=1 /D "_UNICODE" /D "WIN32" /D "STRICT" /D _WIN32_WINNT=0x0400 /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "\Led\Headers" /i "Headers" /i "..\Led\Headers" /d "_DEBUG"
# ADD RSC /l 0x409 /i "\Led\Headers" /i "Headers" /i "..\Led\Headers" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "LedTestWin32 - Win32 Partial UNICODE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LedTestWin32___Win32_Partial_UNICODE_Release"
# PROP BASE Intermediate_Dir "LedTestWin32___Win32_Partial_UNICODE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "LedTestWin32___Win32_Partial_UNICODE_Release"
# PROP Intermediate_Dir "LedTestWin32___Win32_Partial_UNICODE_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GR /GX /O2 /I "Headers" /I "..\Led\Headers" /D "_WINDOWS" /D "NDEBUG" /D qDebug=0 /D "WIN32" /D "_MBCS" /D "STRICT" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /W3 /GR /GX /O2 /I "Headers" /I "..\Led\Headers" /D "_WINDOWS" /D "NDEBUG" /D qDebug=0 /D "_MBCS" /D "qWideCharacters" /D "WIN32" /D "STRICT" /D _WIN32_WINNT=0x0400 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "\Led\Headers" /i "Headers" /i "..\Led\Headers" /d "NDEBUG"
# ADD RSC /l 0x409 /i "\Led\Headers" /i "Headers" /i "..\Led\Headers" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "LedTestWin32 - Win32 Full UNICODE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LedTestWin32___Win32_Full_UNICODE_Release"
# PROP BASE Intermediate_Dir "LedTestWin32___Win32_Full_UNICODE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "LedTestWin32___Win32_Full_UNICODE_Release"
# PROP Intermediate_Dir "LedTestWin32___Win32_Full_UNICODE_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GR /GX /O2 /I "Headers" /I "..\Led\Headers" /D "_WINDOWS" /D "NDEBUG" /D qDebug=0 /D "WIN32" /D "_MBCS" /D "STRICT" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /W3 /GR /GX /O2 /I "Headers" /I "..\Led\Headers" /D "_WINDOWS" /D "NDEBUG" /D qDebug=0 /D "_UNICODE" /D "WIN32" /D "STRICT" /D _WIN32_WINNT=0x0400 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "\Led\Headers" /i "Headers" /i "..\Led\Headers" /d "NDEBUG"
# ADD RSC /l 0x409 /i "\Led\Headers" /i "Headers" /i "..\Led\Headers" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ENDIF 

# Begin Target

# Name "LedTestWin32 - Win32 Release"
# Name "LedTestWin32 - Win32 Debug"
# Name "LedTestWin32 - Win32 Partial UNICODE Debug"
# Name "LedTestWin32 - Win32 Full UNICODE Debug"
# Name "LedTestWin32 - Win32 Partial UNICODE Release"
# Name "LedTestWin32 - Win32 Full UNICODE Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Sources\LedTestWin32.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\LedTestWin32.ico
# End Source File
# Begin Source File

SOURCE=.\Sources\LedTestWin32.rc
# End Source File
# Begin Source File

SOURCE=.\Sources\small.ico
# End Source File
# Begin Source File

SOURCE=.\Sources\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\UnknownEmbeddingIcon.bmp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Headers\LedTestWin32.h
# End Source File
# Begin Source File

SOURCE=.\Headers\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Headers\StdAfx.h
# End Source File
# End Group
# Begin Group "Led Headers"

# PROP Default_Filter ""
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

SOURCE=..\Led\Headers\IdleManager.h
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

SOURCE=..\Led\Headers\SpellCheckEngine.h
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

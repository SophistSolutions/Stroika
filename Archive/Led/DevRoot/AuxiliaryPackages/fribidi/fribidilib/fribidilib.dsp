# Microsoft Developer Studio Project File - Name="fribidilib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=fribidilib - Win32 Full UNICODE Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fribidilib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fribidilib.mak" CFG="fribidilib - Win32 Full UNICODE Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fribidilib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fribidilib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "fribidilib - Win32 Partial UNICODE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fribidilib - Win32 Partial UNICODE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "fribidilib - Win32 Full UNICODE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fribidilib - Win32 Full UNICODE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "fribidilib - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D HAS_FRIBIDI_TAB_CHAR_TYPE_2_I=1 /D HAS_FRIBIDI_TAB_CHAR_TYPE_9_I=1 /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "fribidilib - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D HAS_FRIBIDI_TAB_CHAR_TYPE_2_I=1 /D HAS_FRIBIDI_TAB_CHAR_TYPE_9_I=1 /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "fribidilib - Win32 Partial UNICODE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Partial UNICODE Release"
# PROP BASE Intermediate_Dir "Partial UNICODE Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Partial UNICODE Release"
# PROP Intermediate_Dir "Partial UNICODE Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D HAS_FRIBIDI_TAB_CHAR_TYPE_2_I=1 /D HAS_FRIBIDI_TAB_CHAR_TYPE_9_I=1 /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D HAS_FRIBIDI_TAB_CHAR_TYPE_2_I=1 /D HAS_FRIBIDI_TAB_CHAR_TYPE_9_I=1 /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "fribidilib - Win32 Partial UNICODE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Partial UNICODE Debug"
# PROP BASE Intermediate_Dir "Partial UNICODE Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Partial UNICODE Debug"
# PROP Intermediate_Dir "Partial UNICODE Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D HAS_FRIBIDI_TAB_CHAR_TYPE_2_I=1 /D HAS_FRIBIDI_TAB_CHAR_TYPE_9_I=1 /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D HAS_FRIBIDI_TAB_CHAR_TYPE_2_I=1 /D HAS_FRIBIDI_TAB_CHAR_TYPE_9_I=1 /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "fribidilib - Win32 Full UNICODE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Full UNICODE Release"
# PROP BASE Intermediate_Dir "Full UNICODE Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Full UNICODE Release"
# PROP Intermediate_Dir "Full UNICODE Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D HAS_FRIBIDI_TAB_CHAR_TYPE_2_I=1 /D HAS_FRIBIDI_TAB_CHAR_TYPE_9_I=1 /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D HAS_FRIBIDI_TAB_CHAR_TYPE_2_I=1 /D HAS_FRIBIDI_TAB_CHAR_TYPE_9_I=1 /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "fribidilib - Win32 Full UNICODE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Full UNICODE Debug"
# PROP BASE Intermediate_Dir "Full UNICODE Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Full UNICODE Debug"
# PROP Intermediate_Dir "Full UNICODE Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D HAS_FRIBIDI_TAB_CHAR_TYPE_2_I=1 /D HAS_FRIBIDI_TAB_CHAR_TYPE_9_I=1 /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D HAS_FRIBIDI_TAB_CHAR_TYPE_2_I=1 /D HAS_FRIBIDI_TAB_CHAR_TYPE_9_I=1 /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "fribidilib - Win32 Release"
# Name "fribidilib - Win32 Debug"
# Name "fribidilib - Win32 Partial UNICODE Release"
# Name "fribidilib - Win32 Partial UNICODE Debug"
# Name "fribidilib - Win32 Full UNICODE Release"
# Name "fribidilib - Win32 Full UNICODE Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="..\fribidi-src\fribidi.c"
# End Source File
# Begin Source File

SOURCE="..\fribidi-src\fribidi_char_type.c"
# End Source File
# Begin Source File

SOURCE="..\fribidi-src\fribidi_mem.c"
# End Source File
# Begin Source File

SOURCE="..\fribidi-src\fribidi_mirroring.c"
# End Source File
# Begin Source File

SOURCE="..\fribidi-src\fribidi_types.c"
# End Source File
# Begin Source File

SOURCE="..\fribidi-src\fribidi_utils.c"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project

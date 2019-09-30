StroikaRoot=$(abspath ../../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/Led/Platform/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/Led/Platform/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Frameworks.pdb)
endif

vpath %cpp $(SrcDir)


Objs	=	\


ifeq ($(qFeatureFlag_ATLMFC), use-system)
Objs+= $(ObjDir)MFC_WordProcessor${OBJ_SUFFIX}
Objs+= $(ObjDir)MFC${OBJ_SUFFIX}
endif
ifneq ($(findstring Windows,$(TARGET_PLATFORMS)),)
Objs+= $(ObjDir)Windows${OBJ_SUFFIX}
Objs+= $(ObjDir)Windows_FileRegistration${OBJ_SUFFIX}
endif

ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
CXXFLAGS += /D_WINDOWS 
CXXFLAGS += /DWIN32 
CXXFLAGS += /DSTRICT
CXXFLAGS += /D_WIN32_WINNT=0x0501 
endif

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs)
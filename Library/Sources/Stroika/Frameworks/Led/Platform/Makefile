StroikaRoot:=$(abspath ../../../../../../)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir	:=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/Led/Platform/
ObjDir	:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/Led/Platform/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Frameworks.pdb)
endif

vpath %cpp $(SrcDir)


Objs	:=	\


ifeq ($(qFeatureFlag_ATLMFC), use-system)
Objs+= $(ObjDir)MFC_WordProcessor${OBJ_SUFFIX}
Objs+= $(ObjDir)MFC${OBJ_SUFFIX}
endif
ifneq ($(findstring Windows,$(TARGET_PLATFORMS)),)
Objs+= $(ObjDir)Windows${OBJ_SUFFIX}
Objs+= $(ObjDir)Windows_FileRegistration${OBJ_SUFFIX}
endif


ifeq (Windows,$(findstring Windows,$(TARGET_PLATFORMS)))
# AVOID "_WIN32_WINNT not defined. Defaulting to _WIN32_WINNT_MAXVER (see WinSDKVer.h)"
CXXFLAGS += -D_WIN32_WINNT=0x0501
# Maybe helps avoid bugs?
CXXFLAGS += -DSTRICT
endif

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs)
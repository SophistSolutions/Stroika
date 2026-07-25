StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Frameworks/Led/Platform/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/Led/Platform/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)


Objs	:=	\


ifeq ($(qFeatureFlag_ATLMFC), use-system)
Objs+= $(ObjDir)MFC_WordProcessor${OBJ_SUFFIX}
Objs+= $(ObjDir)MFC${OBJ_SUFFIX}
endif
ifneq ($(findstring Windows,$(TargetPlatforms)),)
Objs+= $(ObjDir)Windows${OBJ_SUFFIX}
Objs+= $(ObjDir)Windows_FileRegistration${OBJ_SUFFIX}
endif


ifeq (Windows,$(findstring Windows,$(TargetPlatforms)))
# AVOID "_WIN32_WINNT not defined. Defaulting to _WIN32_WINNT_MAXVER (see WinSDKVer.h)"
CXXFLAGS += -D_WIN32_WINNT=0x0501
# Maybe helps avoid bugs?
CXXFLAGS += -DSTRICT
endif

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)

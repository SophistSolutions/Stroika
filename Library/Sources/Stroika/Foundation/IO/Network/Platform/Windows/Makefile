StroikaRoot	:=	$(abspath ../../../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/IO/Network/Platform/Windows/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/Network/Platform/Windows/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-foundation.pdb)
endif

SubDirs	:=				\


vpath %cpp $(SrcDir)

Objs	:=	\

ifneq ($(findstring Windows,$(TargetPlatforms)),)
Objs+=	$(ObjDir)WinSock${OBJ_SUFFIX}
endif


all:	$(Objs) $(SubDirs)

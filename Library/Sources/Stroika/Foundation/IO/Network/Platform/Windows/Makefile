StroikaRoot	:=	$(abspath ../../../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/IO/Network/Platform/Windows/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/Network/Platform/Windows/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


SubDirs	:=				\


vpath %cpp $(SrcDir)

Objs	:=	\

ifneq ($(findstring Windows,$(TargetPlatforms)),)
Objs+=	$(ObjDir)WinSock${OBJ_SUFFIX}
endif


all:	$(Objs) $(SubDirs)

StroikaSourceRoot	:=	$(abspath ../../../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/IO/Network/Platform/Windows/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/IO/Network/Platform/Windows/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


SubDirs	:=				\


vpath %cpp $(SrcDir)

Objs	:=	\

ifneq ($(findstring Windows,$(TargetPlatforms)),)
Objs+=	$(ObjDir)WinSock${OBJ_SUFFIX}
endif


all:	$(Objs) $(SubDirs)

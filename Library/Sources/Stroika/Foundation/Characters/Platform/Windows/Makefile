StroikaSourceRoot	:=	$(abspath ../../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Characters/Platform/Windows/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Characters/Platform/Windows/

vpath %cpp $(SrcDir)

Objs	:=

ifneq ($(findstring Windows,$(TargetPlatforms)),)
Objs+=	$(ObjDir)CodePage${OBJ_SUFFIX}
endif


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)


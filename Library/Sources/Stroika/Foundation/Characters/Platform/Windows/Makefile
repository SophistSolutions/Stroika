StroikaRoot	:=	$(abspath ../../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Characters/Platform/Windows/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Characters/Platform/Windows/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)

Objs	:=

ifneq ($(findstring Windows,$(TargetPlatforms)),)
Objs+=	$(ObjDir)CodePage${OBJ_SUFFIX}
endif


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)


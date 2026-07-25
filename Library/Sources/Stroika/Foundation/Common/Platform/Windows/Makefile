StroikaRoot	:=	$(abspath ../../../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Foundation/Common/Platform/Windows/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Common/Platform/Windows/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

Objs	:=

ifneq ($(findstring Windows,$(TargetPlatforms)),)
Objs+=	$(ObjDir)Registry${OBJ_SUFFIX}
endif

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk

all:	 $(Objs)




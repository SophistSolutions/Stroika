StroikaRoot	:=	$(abspath ../../../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Foundation/Execution/Platform/POSIX/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Execution/Platform/POSIX/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk

vpath %cpp $(SrcDir)

Objs	:=

ifneq ($(findstring POSIX,$(TargetPlatforms)),)
Objs+=	$(ObjDir)SemWaitableEvent${OBJ_SUFFIX}
Objs+=	$(ObjDir)SignalBlock${OBJ_SUFFIX}
Objs+=	$(ObjDir)Users${OBJ_SUFFIX}
endif


all:	 $(Objs)




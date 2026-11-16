StroikaSourceRoot	:=	$(abspath ../../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Execution/Platform/POSIX/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Execution/Platform/POSIX/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

vpath %cpp $(SrcDir)

Objs	:=

ifneq ($(findstring POSIX,$(TargetPlatforms)),)
Objs+=	$(ObjDir)SemWaitableEvent${OBJ_SUFFIX}
Objs+=	$(ObjDir)SignalBlock${OBJ_SUFFIX}
Objs+=	$(ObjDir)Users${OBJ_SUFFIX}
endif


all:	 $(Objs)




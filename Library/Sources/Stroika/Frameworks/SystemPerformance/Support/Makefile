StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Frameworks/SystemPerformance/Support/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Frameworks/SystemPerformance/Support/

vpath %cpp $(SrcDir)

Objs	:=	\


ifneq ($(findstring Windows,$(TargetPlatforms)),)
Objs+=	$(ObjDir)WMICollector${OBJ_SUFFIX}
endif

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

all:	$(Objs)


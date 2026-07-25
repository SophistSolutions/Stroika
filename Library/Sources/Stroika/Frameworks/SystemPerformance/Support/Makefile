StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Frameworks/SystemPerformance/Support/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/SystemPerformance/Support/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)

Objs	:=	\


ifneq ($(findstring Windows,$(TargetPlatforms)),)
Objs+=	$(ObjDir)WMICollector${OBJ_SUFFIX}
endif

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk

all:	$(Objs)


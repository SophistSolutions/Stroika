StroikaRoot	:=	$(abspath ../../../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Foundation/Execution/Resources/Concrete/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Execution/Resources/Concrete/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)

Objs	:=

ifneq ($(findstring POSIX,$(TargetPlatforms)),)
Objs+=	$(ObjDir)ElfResourceManager${OBJ_SUFFIX}
endif
ifneq ($(findstring Windows,$(TargetPlatforms)),)
Objs+=	$(ObjDir)WindowsResourceManager${OBJ_SUFFIX}
endif

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk

all:	 $(Objs)




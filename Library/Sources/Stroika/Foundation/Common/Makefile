StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Common/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Common/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)GUID${OBJ_SUFFIX}\
	$(ObjDir)Locale${OBJ_SUFFIX}\
	$(ObjDir)StroikaConfig${OBJ_SUFFIX}\
	$(ObjDir)SystemConfiguration${OBJ_SUFFIX}\
	$(ObjDir)Version${OBJ_SUFFIX}

SubDirs	=	\
	Platform \

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk

all:	$(Objs) $(SubDirs)

.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s


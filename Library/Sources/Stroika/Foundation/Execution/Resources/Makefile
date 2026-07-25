StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Execution/Resources/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Execution/Resources/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Manager${OBJ_SUFFIX}\
	$(ObjDir)Name${OBJ_SUFFIX}\
	$(ObjDir)ResourceNotFoundException${OBJ_SUFFIX}\

SubDirs	:=	\
	Concrete \


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)

.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

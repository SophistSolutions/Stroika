StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Math/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Math/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SubDirs	:=	\
	LinearAlgebra	\
	Optimization	\


vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)Common${OBJ_SUFFIX}\


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)  $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

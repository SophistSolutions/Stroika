include ../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Math/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Math/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


SubDirs	=	\
	LinearAlgebra	\
	Optimization	\

.PHONY:			$(SubDirs)


vpath %cpp $(SrcDir)


Objs	=	\


all:	$(ObjDir) $(Objs)  $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s

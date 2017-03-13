include ../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/Math/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Math/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk




SubDirs	=	\
	LinearAlgebra	\
	Optimization	\

.PHONY:			$(SubDirs)

vpath %cpp $(SrcDir)

Objs	=	\



all:	$(Objs)  $(SubDirs)


LinearAlgebra_OBJS		:= $(shell $(MAKE) --directory LinearAlgebra -s list-objs CONFIGURATION=$(CONFIGURATION))
Optimization_OBJS		:= $(shell $(MAKE) --directory Optimization -s list-objs CONFIGURATION=$(CONFIGURATION))

list-objs::
	@echo $(LinearAlgebra_OBJS) $(Optimization_OBJS)

$(SubDirs):
	@$(MAKE) --directory $@ -s CONFIGURATION=$(CONFIGURATION)

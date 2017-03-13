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


LinearAlgebra_OBJS		:= $(addprefix "LinearAlgebra/", $(shell $(MAKE) --directory LinearAlgebra -s list-objs))
Optimization_OBJS		:= $(addprefix "Optimization/", $(shell $(MAKE) --directory Optimization -s list-objs))

list-objs::
	@echo $(LinearAlgebra_OBJS) $(Optimization_OBJS)

LinearAlgebra:
	@$(MAKE) --directory LinearAlgebra -s

Optimization:
	@$(MAKE) --directory Optimization -s


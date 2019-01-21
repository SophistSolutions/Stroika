include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Math/LinearAlgebra/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Math/LinearAlgebra/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk



vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)Matrix${OBJ_SUFFIX}\
	$(ObjDir)Tensor${OBJ_SUFFIX}\
	$(ObjDir)Vector${OBJ_SUFFIX}\


all:	$(ObjDir) $(Objs)




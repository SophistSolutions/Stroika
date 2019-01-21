include ../../../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=   $(StroikaRoot)Library/Sources/Stroika/Foundation/Execution/Platform/POSIX/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Execution/Platform/POSIX/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)SemWaitableEvent${OBJ_SUFFIX}\
	$(ObjDir)SignalBlock${OBJ_SUFFIX}\
	$(ObjDir)Users${OBJ_SUFFIX}\


all:	$(ObjDir) $(Objs)




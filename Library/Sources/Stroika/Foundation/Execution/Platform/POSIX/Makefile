include ../../../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=   $(StroikaRoot)Library/Sources/Stroika/Foundation/Execution/Platform/POSIX/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Execution/Platform/POSIX/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Users.o\
	$(ObjDir)SemWaitableEvent.o\
	$(ObjDir)SignalBlock.o\


all:	$(ObjDir) $(Objs)




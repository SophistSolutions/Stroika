include ../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/Debug/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Debug/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)Assertions.o\
	$(ObjDir)AssertExternallySynchronizedLock.o\
	$(ObjDir)BackTrace.o\
	$(ObjDir)Debugger.o\
	$(ObjDir)Demangle.o\
	$(ObjDir)Fatal.o\
	$(ObjDir)MallocGuard.o\
	$(ObjDir)Trace.o\


all:	$(ObjDir) $(Objs)
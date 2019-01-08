include ../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/Execution/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Execution/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk



SubDirs	=	\
	Platform/POSIX \


.PHONY:				$(SubDirs)

vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)AbortableMutex.o\
	$(ObjDir)CommandLine.o\
	$(ObjDir)DLLSupport.o\
	$(ObjDir)Exceptions.o\
	$(ObjDir)ErrNoException.o\
	$(ObjDir)FeatureNotSupportedInThisVersionException.o\
	$(ObjDir)Logger.o\
	$(ObjDir)Module.o\
	$(ObjDir)OperationNotSupportedException.o\
	$(ObjDir)Process.o\
	$(ObjDir)ProcessRunner.o\
	$(ObjDir)ProgressMonitor.o\
	$(ObjDir)RequiredComponentMissingException.o\
	$(ObjDir)RequiredComponentVersionMismatchException.o\
	$(ObjDir)SharedStaticData.o\
	$(ObjDir)SilentException.o\
	$(ObjDir)Signals.o\
	$(ObjDir)SignalHandlers.o\
	$(ObjDir)Sleep.o\
	$(ObjDir)SpinLock.o\
	$(ObjDir)Thread.o\
	$(ObjDir)ThreadPool.o\
	$(ObjDir)TimeOutException.o\
	$(ObjDir)UserCanceledException.o\
	$(ObjDir)Users.o\
	$(ObjDir)WaitableEvent.o\
	$(ObjDir)WaitForIOReady.o\


all:	$(ObjDir) $(Objs)  $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s


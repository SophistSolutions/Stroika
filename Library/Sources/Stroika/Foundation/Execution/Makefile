StroikaRoot=$(abspath ../../../../../)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Foundation/Execution/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Execution/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Foundation.pdb)
endif

SubDirs	=	\
	Platform \


.PHONY:				$(SubDirs)

vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)AbortableMutex${OBJ_SUFFIX}\
	$(ObjDir)Activity${OBJ_SUFFIX}\
	$(ObjDir)CommandLine${OBJ_SUFFIX}\
	$(ObjDir)DLLSupport${OBJ_SUFFIX}\
	$(ObjDir)Exceptions${OBJ_SUFFIX}\
	$(ObjDir)FeatureNotSupportedInThisVersionException${OBJ_SUFFIX}\
	$(ObjDir)Logger${OBJ_SUFFIX}\
	$(ObjDir)Module${OBJ_SUFFIX}\
	$(ObjDir)OperationNotSupportedException${OBJ_SUFFIX}\
	$(ObjDir)Process${OBJ_SUFFIX}\
	$(ObjDir)ProcessRunner${OBJ_SUFFIX}\
	$(ObjDir)ProgressMonitor${OBJ_SUFFIX}\
	$(ObjDir)RequiredComponentMissingException${OBJ_SUFFIX}\
	$(ObjDir)RequiredComponentVersionMismatchException${OBJ_SUFFIX}\
	$(ObjDir)SharedStaticData${OBJ_SUFFIX}\
	$(ObjDir)SilentException${OBJ_SUFFIX}\
	$(ObjDir)Signals${OBJ_SUFFIX}\
	$(ObjDir)SignalHandlers${OBJ_SUFFIX}\
	$(ObjDir)Sleep${OBJ_SUFFIX}\
	$(ObjDir)SpinLock${OBJ_SUFFIX}\
	$(ObjDir)Thread${OBJ_SUFFIX}\
	$(ObjDir)ThreadPool${OBJ_SUFFIX}\
	$(ObjDir)Throw${OBJ_SUFFIX}\
	$(ObjDir)TimeOutException${OBJ_SUFFIX}\
	$(ObjDir)Users${OBJ_SUFFIX}\
	$(ObjDir)WaitableEvent${OBJ_SUFFIX}\
	$(ObjDir)WaitForIOReady${OBJ_SUFFIX}\


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs)  $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s


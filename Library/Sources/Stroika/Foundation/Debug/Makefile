StroikaRoot=$(abspath ../../../../../)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Foundation/Debug/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Debug/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Foundation.pdb)
endif

vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)Assertions${OBJ_SUFFIX}\
	$(ObjDir)AssertExternallySynchronizedLock${OBJ_SUFFIX}\
	$(ObjDir)BackTrace${OBJ_SUFFIX}\
	$(ObjDir)Debugger${OBJ_SUFFIX}\
	$(ObjDir)Demangle${OBJ_SUFFIX}\
	$(ObjDir)Fatal${OBJ_SUFFIX}\
	$(ObjDir)MallocGuard${OBJ_SUFFIX}\
	$(ObjDir)Trace${OBJ_SUFFIX}\


all:	$(Objs)
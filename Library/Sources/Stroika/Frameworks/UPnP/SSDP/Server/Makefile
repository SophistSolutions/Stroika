StroikaRoot=$(abspath ../../../../../../../)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/UPnP/SSDP/Server/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/UPnP/SSDP/Server/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Frameworks.pdb)
endif


Objs	=	\
	$(ObjDir)BasicServer${OBJ_SUFFIX}\
	$(ObjDir)PeriodicNotifier${OBJ_SUFFIX}\
	$(ObjDir)SearchResponder${OBJ_SUFFIX}\


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs)
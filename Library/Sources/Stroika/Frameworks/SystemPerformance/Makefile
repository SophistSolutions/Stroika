StroikaRoot=$(abspath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/SystemPerformance/
ObjDir	 =	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/SystemPerformance/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Frameworks.pdb)
endif


SubDirs	=	\
	Instruments	\
	Support	\


vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)AllInstruments${OBJ_SUFFIX}\
	$(ObjDir)Capturer${OBJ_SUFFIX}\
	$(ObjDir)CaptureSet${OBJ_SUFFIX}\
	$(ObjDir)Instrument${OBJ_SUFFIX}\
	$(ObjDir)Measurement${OBJ_SUFFIX}\
	$(ObjDir)MeasurementSet${OBJ_SUFFIX}\


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

include ../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir						=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/SystemPerformance/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/SystemPerformance/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


SubDirs	=	\
	Instruments	\
	Support	\

.PHONY:			$(SubDirs)


vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)AllInstruments${OBJ_SUFFIX}\
	$(ObjDir)Capturer${OBJ_SUFFIX}\
	$(ObjDir)CaptureSet${OBJ_SUFFIX}\
	$(ObjDir)Instrument${OBJ_SUFFIX}\
	$(ObjDir)Measurement${OBJ_SUFFIX}\
	$(ObjDir)MeasurementSet${OBJ_SUFFIX}\


all:	$(ObjDir) $(Objs) $(SubDirs)


Instruments:
	@$(MAKE) --directory Instruments -s

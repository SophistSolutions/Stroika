include ../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir						=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/SystemPerformance/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/SystemPerformance/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


SubDirs	=	\
	Instruments	\
	Support	\

.PHONY:			$(SubDirs)


vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)AllInstruments.o\
	$(ObjDir)Capturer.o\
	$(ObjDir)CaptureSet.o\
	$(ObjDir)Instrument.o\
	$(ObjDir)InstrumentSet.o\
	$(ObjDir)Measurement.o\
	$(ObjDir)MeasurementSet.o\


all:	$(ObjDir) $(Objs) $(SubDirs)


list-objs::
	@echo \
		$(shell $(MAKE) --directory Instruments -s list-objs CONFIGURATION=$(CONFIGURATION))


Instruments:
	@$(MAKE) --directory Instruments -s CONFIGURATION=$(CONFIGURATION)

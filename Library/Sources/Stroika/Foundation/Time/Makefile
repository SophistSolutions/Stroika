include ../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=   $(StroikaRoot)Library/Sources/Stroika/Foundation/Time/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Time/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Date.o\
	$(ObjDir)DateTime.o\
	$(ObjDir)Duration.o\
	$(ObjDir)DurationRange.o\
	$(ObjDir)Realtime.o\
	$(ObjDir)TimeOfDay.o\
	$(ObjDir)Timezone.o\


all:	$(ObjDir) $(Objs)


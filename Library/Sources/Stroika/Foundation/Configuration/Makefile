include ../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir			=   $(StroikaRoot)Library/Sources/Stroika/Foundation/Configuration/
ObjDir			=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Configuration/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Locale.o\
	$(ObjDir)StroikaConfig.o\
	$(ObjDir)SystemConfiguration.o\
	$(ObjDir)Version.o


all:	$(ObjDir) $(Objs)

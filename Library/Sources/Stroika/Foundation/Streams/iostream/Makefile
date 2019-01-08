include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=   $(StroikaRoot)Library/Sources/Stroika/Foundation/Streams/iostream/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Streams/iostream/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)CommonInsertersAndExtractors.o\
	$(ObjDir)FStreamSupport.o\
	$(ObjDir)SerializeItemToBLOB.o\
	$(ObjDir)Utilities.o\
	

all:	$(ObjDir) $(Objs)

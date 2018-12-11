include ../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir						=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cache/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cache/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Statistics.o


all:	$(ObjDir) $(Objs)

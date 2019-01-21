include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir						=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/SystemPerformance/Instruments/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/SystemPerformance/Instruments/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)CPU${OBJ_SUFFIX}\
	$(ObjDir)Memory${OBJ_SUFFIX}\
	$(ObjDir)Filesystem${OBJ_SUFFIX}\
	$(ObjDir)Network${OBJ_SUFFIX}\
	$(ObjDir)Process${OBJ_SUFFIX}\


all:	$(ObjDir) $(Objs)


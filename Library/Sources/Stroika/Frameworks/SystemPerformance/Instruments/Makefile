StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/SystemPerformance/Instruments/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/SystemPerformance/Instruments/

include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-frameworks.pdb)
endif


vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)CPU${OBJ_SUFFIX}\
	$(ObjDir)Memory${OBJ_SUFFIX}\
	$(ObjDir)Filesystem${OBJ_SUFFIX}\
	$(ObjDir)Network${OBJ_SUFFIX}\
	$(ObjDir)Process${OBJ_SUFFIX}\


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs)


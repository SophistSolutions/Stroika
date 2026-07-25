StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/SystemPerformance/Instruments/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/SystemPerformance/Instruments/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)CPU${OBJ_SUFFIX}\
	$(ObjDir)Memory${OBJ_SUFFIX}\
	$(ObjDir)Filesystem${OBJ_SUFFIX}\
	$(ObjDir)Network${OBJ_SUFFIX}\
	$(ObjDir)Process${OBJ_SUFFIX}\


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)


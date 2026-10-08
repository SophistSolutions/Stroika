StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Frameworks/UPnP/GENA/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/UPnP/GENA/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)Publisher${OBJ_SUFFIX}\
	$(ObjDir)Subscriber${OBJ_SUFFIX}\

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)

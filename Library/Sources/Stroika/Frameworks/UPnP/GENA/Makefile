StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Frameworks/UPnP/GENA/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Frameworks/UPnP/GENA/

vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)Publisher${OBJ_SUFFIX}\
	$(ObjDir)Subscriber${OBJ_SUFFIX}\

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)

StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Streams/iostream/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Streams/iostream/

vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)FStreamSupport${OBJ_SUFFIX}\
	$(ObjDir)Utilities${OBJ_SUFFIX}\
	

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)

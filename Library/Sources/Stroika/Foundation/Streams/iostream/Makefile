StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Foundation/Streams/iostream/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Streams/iostream/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)FStreamSupport${OBJ_SUFFIX}\
	$(ObjDir)Utilities${OBJ_SUFFIX}\
	

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)

StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Linguistics/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Linguistics/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk

vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)MessageUtilities${OBJ_SUFFIX}\
	$(ObjDir)Words${OBJ_SUFFIX}\


all:	$(Objs)


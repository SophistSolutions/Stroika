StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Frameworks/Service/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/Service/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk



vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)Main${OBJ_SUFFIX}\


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)

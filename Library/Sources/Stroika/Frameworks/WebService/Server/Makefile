StroikaRoot=$(abspath ../../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/WebService/Server/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/WebService/Server/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)Basic${OBJ_SUFFIX}\
	$(ObjDir)VariantValue${OBJ_SUFFIX}\


all:	$(Objs) $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s

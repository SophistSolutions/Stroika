StroikaRoot=$(abspath ../../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/Compression/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/Compression/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Reader${OBJ_SUFFIX}\

SubDirs	=	\
	Zip \


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


.PHONY:				$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

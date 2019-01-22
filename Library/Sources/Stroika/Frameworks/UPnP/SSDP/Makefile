StroikaRoot=$(abspath ../../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/UPnP/SSDP/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/UPnP/SSDP/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

SubDirs	=	\
	Client	\
	Server	\


Objs	=	\
	$(ObjDir)Advertisement${OBJ_SUFFIX}\
	$(ObjDir)Common${OBJ_SUFFIX}\


all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

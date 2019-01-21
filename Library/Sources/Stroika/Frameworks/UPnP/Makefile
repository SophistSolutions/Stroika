include ../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir						=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/UPnP/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/UPnP/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)


SubDirs	=	\
	SSDP	\

.PHONY:			$(SubDirs)


Objs	=	\
	$(ObjDir)Device${OBJ_SUFFIX}\
	$(ObjDir)DeviceDescription${OBJ_SUFFIX}\


all:	$(ObjDir) $(Objs) $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s

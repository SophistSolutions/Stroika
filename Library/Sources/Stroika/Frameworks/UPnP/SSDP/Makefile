include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir						=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/UPnP/SSDP/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/UPnP/SSDP/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

SubDirs	=	\
	Client	\
	Server	\

.PHONY:			$(SubDirs)

Objs	=	\
	$(ObjDir)Advertisement.o\
	$(ObjDir)Common.o\


all:	$(ObjDir) $(Objs) $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s

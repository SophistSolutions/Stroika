include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir						=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/UPnP/SSDP/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/UPnP/SSDP/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

SubDirs	=	\
	Client	\
	Server	\

.PHONY:			$(SubDirs)

Objs	=	\
	$(ObjDir)Advertisement.o\
	$(ObjDir)Common.o\


all:	$(ObjDir) $(Objs) $(SubDirs)


list-objs::
	@echo \
		$(shell $(MAKE) --directory Client -s list-objs CONFIGURATION=$(CONFIGURATION))\
		$(shell $(MAKE) --directory Server -s list-objs CONFIGURATION=$(CONFIGURATION))


$(SubDirs):
	@$(MAKE) --directory $@ -s  CONFIGURATION=$(CONFIGURATION)

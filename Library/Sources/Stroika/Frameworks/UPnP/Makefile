include ../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir						=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/UPnP/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/UPnP/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)


SubDirs	=	\
	SSDP	\

.PHONY:			$(SubDirs)


Objs	=	\
	$(ObjDir)Device.o\
	$(ObjDir)DeviceDescription.o\


all:	$(Objs) $(SubDirs)


list-objs::
	@echo \
		$(shell $(MAKE) --directory SSDP -s list-objs  CONFIGURATION=$(CONFIGURATION))


$(SubDirs):
	@$(MAKE) --directory $@ -s  CONFIGURATION=$(CONFIGURATION)

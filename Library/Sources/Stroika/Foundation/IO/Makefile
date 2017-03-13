include ../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/IO/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



SubDirs	=	\
	FileSystem	\
	Network		\

.PHONY:			$(SubDirs)


vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)FileAccessException.o\
	$(ObjDir)FileAccessMode.o\
	$(ObjDir)FileBusyException.o\
	$(ObjDir)FileFormatException.o\


all:	$(ObjDir) $(Objs) $(SubDirs)


list-objs::
	@echo \
		$(shell $(MAKE) --directory FileSystem -s list-objs CONFIGURATION=$(CONFIGURATION))\
		$(shell $(MAKE) --directory Network -s list-objs CONFIGURATION=$(CONFIGURATION))


$(SubDirs):
	@$(MAKE) --directory $@ -s CONFIGURATION=$(CONFIGURATION)


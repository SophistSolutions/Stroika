include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/Archive/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/Archive/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk




vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Reader.o\

SubDirs	=	\
	7z \
	Zip \

.PHONY:				$(SubDirs)


all:	$(ObjDir) $(Objs) $(SubDirs)


list-objs::
	@echo \
		$(shell $(MAKE) --directory 7z -s list-objs CONFIGURATION=$(CONFIGURATION))\
		$(shell $(MAKE) --directory Zip -s list-objs CONFIGURATION=$(CONFIGURATION))


$(SubDirs):
	@$(MAKE) --directory $@ -s CONFIGURATION=$(CONFIGURATION)

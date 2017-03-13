include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/Variant/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/Variant/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Reader.o\
	$(ObjDir)Writer.o\

SubDirs	=	\
	CharacterDelimitedLines \
	INI \
	JSON \
	XML \

.PHONY:				$(SubDirs)


all:	$(ObjDir) $(Objs) $(SubDirs)


list-objs::
	@echo \
		$(shell $(MAKE) --directory CharacterDelimitedLines -s list-objs CONFIGURATION=$(CONFIGURATION))\
		$(shell $(MAKE) --directory INI -s list-objs CONFIGURATION=$(CONFIGURATION))\
		$(shell $(MAKE) --directory JSON -s list-objs CONFIGURATION=$(CONFIGURATION))\
		$(shell $(MAKE) --directory XML -s list-objs CONFIGURATION=$(CONFIGURATION))\


$(SubDirs):
	@$(MAKE) --directory $@ -s CONFIGURATION=$(CONFIGURATION) CONFIGURATION=$(CONFIGURATION)

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


all:	$(Objs) $(SubDirs)


CharacterDelimitedLines_OBJS := $(shell $(MAKE) --directory CharacterDelimitedLines -s list-objs CONFIGURATION=$(CONFIGURATION))
INI_OBJS := $(shell $(MAKE) --directory INI -s list-objs CONFIGURATION=$(CONFIGURATION))
JSON_OBJS :=  $(shell $(MAKE) --directory JSON -s list-objs CONFIGURATION=$(CONFIGURATION))
XML_OBJS := $(shell $(MAKE) --directory XML -s list-objs CONFIGURATION=$(CONFIGURATION))


list-objs::
	@echo $(CharacterDelimitedLines_OBJS) $(INI_OBJS) $(JSON_OBJS) $(XML_OBJS)


$(SubDirs):
	@$(MAKE) --directory $@ -s CONFIGURATION=$(CONFIGURATION) CONFIGURATION=$(CONFIGURATION)

include ../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Atom.o\
	$(ObjDir)BadFormatException.o\
	$(ObjDir)CheckedConverter.o\
	$(ObjDir)InternetMediaType.o\
	$(ObjDir)ObjectVariantMapper.o\
	$(ObjDir)OptionsFile.o\
	$(ObjDir)StructFieldMetaInfo.o\
	$(ObjDir)VariantValue.o\

SubDirs	=	\
	Archive \
	Compression \
	Variant \
	StructuredStreamEvents \
	XML \

.PHONY:				$(SubDirs)


all:	$(ObjDir) $(Objs) $(SubDirs)


list-objs::
	@echo \
		$(shell $(MAKE) --directory Archive -s list-objs CONFIGURATION=$(CONFIGURATION))\
		$(shell $(MAKE) --directory Compression -s list-objs CONFIGURATION=$(CONFIGURATION))\
		$(shell $(MAKE) --directory Variant -s list-objs CONFIGURATION=$(CONFIGURATION))\
		$(shell $(MAKE) --directory StructuredStreamEvents -s list-objs CONFIGURATION=$(CONFIGURATION))\
		$(shell $(MAKE) --directory XML -s list-objs CONFIGURATION=$(CONFIGURATION))\


$(SubDirs):
	@$(MAKE) --directory $@ -s CONFIGURATION=$(CONFIGURATION)

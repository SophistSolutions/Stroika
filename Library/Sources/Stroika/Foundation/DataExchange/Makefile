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


all:	$(Objs) $(SubDirs)


Archive_OBJS := $(addprefix "Archive/", $(shell $(MAKE) --directory Archive -s list-objs CONFIGURATION=$(CONFIGURATION)))
Compression_OBJS := $(addprefix "Compression/", $(shell $(MAKE) --directory Compression -s list-objs CONFIGURATION=$(CONFIGURATION)))
Variant_OBJS := $(addprefix "Variant/", $(shell $(MAKE) --directory Variant -s list-objs CONFIGURATION=$(CONFIGURATION)))
StructuredStreamEvents_OBJS := $(addprefix "StructuredStreamEvents/", $(shell $(MAKE) --directory StructuredStreamEvents -s list-objs CONFIGURATION=$(CONFIGURATION)))
XML_OBJS := $(addprefix "XML/", $(shell $(MAKE) --directory XML -s list-objs CONFIGURATION=$(CONFIGURATION)))


list-objs::
	@echo $(Archive_OBJS) $(Compression_OBJS) $(Variant_OBJS) $(StructuredStreamEvents_OBJS) $(XML_OBJS)


$(SubDirs):
	@$(MAKE) --directory $@ -s CONFIGURATION=$(CONFIGURATION)

include ../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

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
	$(ObjDir)InternetMediaTypeRegistry.o\
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


$(SubDirs):
	@$(MAKE) --directory $@ -s

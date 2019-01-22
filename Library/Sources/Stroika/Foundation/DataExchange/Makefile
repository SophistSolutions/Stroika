export StroikaRoot?=$(shell realpath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk



vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Atom${OBJ_SUFFIX}\
	$(ObjDir)BadFormatException${OBJ_SUFFIX}\
	$(ObjDir)CheckedConverter${OBJ_SUFFIX}\
	$(ObjDir)InternetMediaType${OBJ_SUFFIX}\
	$(ObjDir)InternetMediaTypeRegistry${OBJ_SUFFIX}\
	$(ObjDir)ObjectVariantMapper${OBJ_SUFFIX}\
	$(ObjDir)OptionsFile${OBJ_SUFFIX}\
	$(ObjDir)StructFieldMetaInfo${OBJ_SUFFIX}\
	$(ObjDir)VariantValue${OBJ_SUFFIX}\

SubDirs	=	\
	Archive \
	Compression \
	Variant \
	StructuredStreamEvents \
	XML \

.PHONY:				$(SubDirs)


all:	$(Objs) $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s

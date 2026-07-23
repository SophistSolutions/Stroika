StroikaRoot	:=	$(abspath ../../../../../)/

SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/

include $(StroikaRoot)Build/ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-foundation.pdb)
endif

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Atom${OBJ_SUFFIX}\
	$(ObjDir)BadFormatException${OBJ_SUFFIX}\
	$(ObjDir)InternetMediaType${OBJ_SUFFIX}\
	$(ObjDir)InternetMediaTypeRegistry${OBJ_SUFFIX}\
	$(ObjDir)ObjectVariantMapper${OBJ_SUFFIX}\
	$(ObjDir)OptionsFile${OBJ_SUFFIX}\
	$(ObjDir)StructFieldMetaInfo${OBJ_SUFFIX}\
	$(ObjDir)TypedBLOB${OBJ_SUFFIX}\
	$(ObjDir)VariantValue${OBJ_SUFFIX}\

SubDirs	:=	\
	Archive \
	Compression \
	JSON \
	Variant \
	StructuredStreamEvents \
	XML \

include $(StroikaRoot)Build/ScriptsLib/SharedBuildRules-Default.mk

all:	$(Objs) $(SubDirs)


.PHONY:				$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

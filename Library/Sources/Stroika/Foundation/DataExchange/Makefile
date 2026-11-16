StroikaSourceRoot	:=	$(abspath ../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/DataExchange/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/DataExchange/

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

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

all:	$(Objs) $(SubDirs)


.PHONY:				$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

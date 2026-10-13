StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Frameworks/UPnP/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/UPnP/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)

# so the *DescriptionSchema.cpp files can include the .embed generated beside their objects (as WebService's Makefile does)
CPPFLAGS	+=	-I${ObjDir_ToolsSafe}


SubDirs	:=	\
	GENA	\
	SOAP	\
	SSDP	\



Objs	=	\
	$(ObjDir)DataTypes${OBJ_SUFFIX}\
	$(ObjDir)Device${OBJ_SUFFIX}\
	$(ObjDir)DeviceDescription${OBJ_SUFFIX}\
	$(ObjDir)DeviceDescriptionSchema${OBJ_SUFFIX}\
	$(ObjDir)ServiceDescription${OBJ_SUFFIX}\
	$(ObjDir)ServiceDescriptionSchema${OBJ_SUFFIX}\

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk

# Vendored schemas, embedded as BYTES - never as a C++ string literal: MSVC is passed no /utf-8, so any non-ASCII in
# someone else's file would be read as cp1252.
# order-only $(ObjDir): with -j, nothing else guarantees the directory exists before xxd writes into it
$(ObjDir)%.embed:	$(SrcDir)Schemas/% | $(ObjDir)
	cd $(SrcDir) && xxd -i Schemas/$* $@

$(ObjDir)DeviceDescriptionSchema${OBJ_SUFFIX}:	$(ObjDir)device-1-0.xsd.embed
$(ObjDir)ServiceDescriptionSchema${OBJ_SUFFIX}:	$(ObjDir)service-1-0.xsd.embed


all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

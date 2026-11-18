# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
StroikaSourceRoot	:=	$(abspath ../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Frameworks/WebService/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Frameworks/WebService/

SubDirs	:=	\
	Client	\
	OpenAPI	\
	Server	\


vpath %cpp $(SrcDir)

# so SOAP_EnvelopeSchema.cpp can include the .embed generated beside its object (as Tests/Makefile-Test-Template.mk does)
CPPFLAGS	+=	-I${ObjDir_ToolsSafe}


Objs	:=	\
	$(ObjDir)JSONRPC${OBJ_SUFFIX}	\
	$(ObjDir)SOAP${OBJ_SUFFIX}	\
	$(ObjDir)SOAP_EnvelopeSchema${OBJ_SUFFIX}	\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

# A vendored schema, embedded as BYTES - never as a C++ string literal: they carry non-ASCII (a copyright sign in the
# notice their licence requires be kept) which MSVC, passed no /utf-8, would read as cp1252.
# order-only $(ObjDir): with -j, nothing else guarantees the directory exists before xxd writes into it
$(ObjDir)%.embed:	$(SrcDir)Schemas/% | $(ObjDir)
	cd $(SrcDir) && xxd -i Schemas/$* $@

$(ObjDir)SOAP_EnvelopeSchema${OBJ_SUFFIX}:	$(ObjDir)soap-envelope-1.1.xsd.embed

all:	$(Objs)  $(SubDirs)

.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

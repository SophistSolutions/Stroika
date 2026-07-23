StroikaRoot	:=	$(abspath ../../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/IO/Network/HTTP/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/Network/HTTP/

include $(StroikaRoot)Build/ScriptsLib/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/ScriptsLib/SharedBuildRules-Default.mk


ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-foundation.pdb)
endif

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)CacheControl${OBJ_SUFFIX}\
	$(ObjDir)ClientErrorException${OBJ_SUFFIX}\
	$(ObjDir)ContentEncoding${OBJ_SUFFIX}\
	$(ObjDir)Cookie${OBJ_SUFFIX}\
	$(ObjDir)Exception${OBJ_SUFFIX}\
	$(ObjDir)Headers${OBJ_SUFFIX}\
	$(ObjDir)KeepAlive${OBJ_SUFFIX}\
	$(ObjDir)MessageStartTextInputStreamBinaryAdapter${OBJ_SUFFIX}\
	$(ObjDir)Request${OBJ_SUFFIX}\
	$(ObjDir)Response${OBJ_SUFFIX}\
	$(ObjDir)TransferEncoding${OBJ_SUFFIX}\


all:	$(Objs)


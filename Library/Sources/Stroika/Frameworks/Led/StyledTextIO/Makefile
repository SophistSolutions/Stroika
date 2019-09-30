StroikaRoot=$(abspath ../../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/Led/StyledTextIO/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/Led/StyledTextIO/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Frameworks.pdb)
endif

vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)StyledTextIO${OBJ_SUFFIX}\
	$(ObjDir)StyledTextIO_HTML${OBJ_SUFFIX}\
	$(ObjDir)StyledTextIO_LedNative${OBJ_SUFFIX}\
	$(ObjDir)StyledTextIO_MIMETextEnriched${OBJ_SUFFIX}\
	$(ObjDir)StyledTextIO_PlainText${OBJ_SUFFIX}\
	$(ObjDir)StyledTextIO_RTF${OBJ_SUFFIX}\
	$(ObjDir)StyledTextIO_STYLText${OBJ_SUFFIX}\


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs)
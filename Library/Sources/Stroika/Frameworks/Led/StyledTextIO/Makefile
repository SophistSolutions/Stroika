StroikaRoot=$(abspath ../../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/Led/StyledTextIO/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/Led/StyledTextIO/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Frameworks.pdb)
endif

vpath %cpp $(SrcDir)

Objs	=	

# for now - this only builds on windows, cuz unix requires x-windows, and macos only written for very old macos gdi (carbon)
ifneq ($(findstring Windows,$(TARGET_PLATFORMS)),)
Objs+=	$(ObjDir)StyledTextIO${OBJ_SUFFIX}
Objs+=	$(ObjDir)StyledTextIO_HTML${OBJ_SUFFIX}
Objs+=	$(ObjDir)StyledTextIO_LedNative${OBJ_SUFFIX}
Objs+=	$(ObjDir)StyledTextIO_MIMETextEnriched${OBJ_SUFFIX}
Objs+=	$(ObjDir)StyledTextIO_PlainText${OBJ_SUFFIX}
Objs+=	$(ObjDir)StyledTextIO_RTF${OBJ_SUFFIX}
Objs+=	$(ObjDir)StyledTextIO_STYLText${OBJ_SUFFIX}
endif

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs)
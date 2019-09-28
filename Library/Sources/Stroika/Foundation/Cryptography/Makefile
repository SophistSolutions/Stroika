StroikaRoot=$(abspath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Foundation.pdb)
endif

SubDirs	=		\
	Digest		\
	Encoding	\
	OpenSSL		\
	SSL			\


vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)Hash${OBJ_SUFFIX}\
	$(ObjDir)Format${OBJ_SUFFIX}\

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

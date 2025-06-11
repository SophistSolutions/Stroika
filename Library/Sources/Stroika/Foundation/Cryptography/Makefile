StroikaRoot	:=	$(abspath ../../../../../)/

SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/

include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-foundation.pdb)
endif

SubDirs	:=		\
	Digest		\
	Encoding	\
	Providers	\
	PKI			\
	SSL			\


vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)Format${OBJ_SUFFIX}\

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

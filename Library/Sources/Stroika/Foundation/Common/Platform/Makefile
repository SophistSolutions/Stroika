StroikaRoot	:=	$(abspath ../../../../../../)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir	:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Common/Platform/
ObjDir	:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Common/Platform/

include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-foundation.pdb)
endif

vpath %cpp $(SrcDir)

Objs	:=	\

SubDirs	:=	\
	Windows \


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)

.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

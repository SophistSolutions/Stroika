StroikaRoot	:=	$(abspath ../../../../../../)/

SrcDir	:=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/WebService/Server/
ObjDir	:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/WebService/OpenAPI/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-frameworks.pdb)
endif


vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)Specification${OBJ_SUFFIX}\


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s

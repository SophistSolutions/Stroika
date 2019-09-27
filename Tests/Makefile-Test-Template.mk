export StroikaRoot?=$(abspath ../../)/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir				=	$(StroikaRoot)Tests/$(TEST_NUM)/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tests/$(TEST_NUM)/

include $(StroikaRoot)/ScriptsLib/SharedBuildRules-Default.mk
include $(StroikaRoot)/ScriptsLib/SharedMakeVariables-Default.mk


ifndef TEST_NUM
	TEST_NUM		=	_UNDEFINED_FIX_INCLUDER_OF_TEMPLATE_TO_SPECIFY_THIS_
endif


TARGETEXE				=	$(StroikaRoot)Builds/$(CONFIGURATION)/Tests/Test$(TEST_NUM)${EXE_SUFFIX}

vpath %.h $(SrcDir) ../TestHarness/
vpath %.cpp $(SrcDir) ../TestHarness/


Objs	=	\
	$(ObjDir)Test${OBJ_SUFFIX}\
	$(ObjDir)NotCopyable${OBJ_SUFFIX}\
	$(ObjDir)SimpleClass${OBJ_SUFFIX}\
	$(ObjDir)TestHarness${OBJ_SUFFIX}\


all:	$(ObjDir) $(TARGETEXE)


check:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Test $(TEST_NUM): $(shell $(StroikaRoot)Tests/ScriptsLib/PrintTestName $(TEST_NUM)) :  "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


#tmphack - til we can properly amend configure (and to experiment HOW we need to amend confiugure)
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))

EXTRA_PREFIX_LINKER_ARGS+= /MACHINE:${WIN_LIBCOMPATIBLE_ARCH}

#FOR RELEASE BUIILDS 
#	/OPT:REF eliminates functions and or data that are never referenced
#	/OPT:ICF[=iterations] to perform identical COMDAT folding
EXTRA_PREFIX_LINKER_ARGS+= /OPT:REF
EXTRA_PREFIX_LINKER_ARGS+= /OPT:ICF

EXTRA_PREFIX_LINKER_ARGS+= /DEBUG
#EXTRA_PREFIX_LINKER_ARGS+= /DYNAMICBASE:NO

#EXTRA_SUFFIX_LINKER_ARGS+= /SAFESEH 
#EXTRA_SUFFIX_LINKER_ARGS+= /INCREMENTAL:NO
#EXTRA_SUFFIX_LINKER_ARGS+= /PGD:"C:\Sandbox\Stroika\DevRoot\Tests\Projects\VisualStudio.Net-2019\34\..\..\..\..\Builds\Release-U-32\Tests\Test34.pgd" 

# 
# /LTCG linktime code gen
# /LTCG:incremental incremental linktime codegen (or many other : variations)
#EXTRA_PREFIX_LINKER_ARGS+= /LTCG
endif



# Cannot be done generically through configuration cuz a given build of Stroika might contain EXEs targetting multiple subsystems
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
EXTRA_SUFFIX_LINKER_ARGS+= /SUBSYSTEM:CONSOLE
endif


$(TARGETEXE):	$(Objs) $(StroikaLibs)
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "Linking `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@`" "... "
	@mkdir -p $(dir $@)
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) $(call DEFAULT_LINK_LINE,$@);\
	fi
	@$(call DEFAULT_LINK_LINE,$@)

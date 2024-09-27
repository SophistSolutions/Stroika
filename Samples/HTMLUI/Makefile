StroikaRoot:=$(abspath ../../)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

SrcDir			:=	$(StroikaRoot)Samples/HTMLUI/
ObjDir			:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-HTMLUI/

#not parallel because submakefiles use parallelism, but generally best to sequence these top level requests. Like if you say
# make clobber all you don't want those to happen at the same time. And make libraries samples wouldn't really work since all the libraries
# have to be built before the samples etc...
.NOTPARALLEL:


# CXXFLAGS				+=	-I$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-HTMLUI/)

TARGETDIR		:=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples-HTMLUI/
# TARGETEXE		:=	$(TARGETDIR)HTMLUI${EXE_SUFFIX}


# VPATH	=			$(SrcDir)/Sources/



include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk



all :
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/HTMLUI:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent --directory Backend MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@

 check:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "$(call FUNCTION_CAPITALIZE_WORD,$@) Samples/HTMLUI:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent --directory Backend MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@

clean::
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "$(call FUNCTION_CAPITALIZE_WORD,$@) Samples/HTMLUI:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent --directory Backend MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@


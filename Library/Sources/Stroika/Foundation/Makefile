StroikaRoot=$(abspath ../../../../)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


VPATH		=	$(SrcDir) $(StroikaLibDir)

SubDirs=				\
	Cache				\
	Characters			\
	Common				\
	Configuration		\
	Containers			\
	Cryptography		\
	Database			\
	DataExchange		\
	Debug				\
	Execution			\
	IO					\
	Linguistics			\
	Math				\
	Memory				\
	Streams				\
	Time				\
	Traversal			\


# redirect errors on list-objs cuz could fail due to directories not existing yet (#include) and not worth fixing each
# sub-make - easier to just ignore in this one case from here
# -- LGP 2019-01-09
Objs := $(foreach dir, $(SubDirs), $(shell $(MAKE) --directory $(dir) CONFIGURATION=$(CONFIGURATION) -s list-objs 2>/dev/null))

$(Objs):	all_objs_


$(ObjDir)%${OBJ_SUFFIX} : %.cpp
	@#suppress normal rule so just all_objs_ fires


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s


.PHONY: all_objs_
all_objs_:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika Foundation Objs:";
	@$(MAKE) --no-print-directory -s $(SubDirs) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


$(StroikaFoundationLib):	$(Objs)
	@mkdir -p $(StroikaLibDir)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Creating `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@` ... "
	@rm -f $@
	@if [ "$(AR)" != "" ]; then \
		if [ $(ECHO_BUILD_LINES) -eq 1 ]; then \
			echo "      "$(AR) cr $@ $(Objs);\
		fi;\
		$(AR) cr $@ $(Objs);\
	fi
	@if [ "$(RANLIB)" != "" ]; then \
		if [ $(ECHO_BUILD_LINES) -eq 1 ]; then \
			echo "      "$(RANLIB) $@;\
		fi;\
		$(RANLIB) $@;\
	fi
	@if [ $(LIBTOOL) != "" ]; then \
		if [ $(ECHO_BUILD_LINES) -eq 1 ]; then \
			echo "      "$(LIBTOOL) /OUT:$(shell cygpath --mixed $(StroikaFoundationLib)) /MACHINE:X86 /NOLOGO $(shell cygpath --mixed $(Objs));\
		fi;\
		$(LIBTOOL) /OUT:$(shell cygpath --mixed $(StroikaFoundationLib)) /MACHINE:X86 /NOLOGO $(shell cygpath --mixed $(Objs));\
	fi



ifeq ($(USE_MSBUILD),0)
all:	$(StroikaFoundationLib)
else
all:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika-Foundation:"
	@$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../Projects/$(ProjectPlatformSubdir)/Stroika-Foundation.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";
endif


check:
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(StroikaFoundationLib)
	@$(ECHO) "[SUCCEEDED]"


clean::
ifeq ($(CONFIGURATION),)
	@rm -rf $(StroikaRoot)IntermediateFiles/*/Library/Foundation
else
	@rm -rf $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation
endif

clobber::
ifeq (Unix,$(ProjectPlatformSubdir))
	@rm -f $(StroikaFoundationLib)
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../Projects/$(ProjectPlatformSubdir)/Stroika-Foundation.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Clean";
endif

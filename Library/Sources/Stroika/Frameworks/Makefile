StroikaRoot=$(abspath ../../../../)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Frameworks/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Frameworks.pdb
endif


VPATH		=	$(SrcDir) $(StroikaLibDir)


SubDirs=	\
		Modbus\
		NetworkMonitor\
		Service\
		SystemPerformance\
		WebServer\
		WebService\
		UPnP\



.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s


# redirect errors on list-objs cuz could fail due to directories not existing yet (#include) and not worth fixing each
# sub-make - easier to just ignore in this one case from here
# -- LGP 2019-01-09
Objs := $(foreach dir, $(SubDirs), $(shell $(MAKE) --directory $(dir) CONFIGURATION=$(CONFIGURATION) -s list-objs 2>/dev/null))

$(ObjDir)%${OBJ_SUFFIX} : %.cpp
	@#suppress normal rule so just all_objs_ fires


$(Objs): all_objs_

.PHONY:	all_objs_
all_objs_:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika Frameworks Objs:";
	@$(MAKE) --no-print-directory -s $(SubDirs) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


$(StroikaFrameworksLib):	$(Objs)
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
			echo "      "$(LIBTOOL) /OUT:$(shell cygpath --mixed $(StroikaFrameworksLib)) /MACHINE:X86 /NOLOGO $(shell cygpath --mixed $(Objs));\
		fi;\
		$(LIBTOOL) /OUT:$(shell cygpath --mixed $(StroikaFrameworksLib)) /MACHINE:X86 /NOLOGO $(shell cygpath --mixed $(Objs));\
	fi



## experimenting/moving towards build without msbuild (just makefile build)
ifeq (Unix,$(ProjectPlatformSubdir))
USE_MSBUILD_=0
else
#USE_MSBUILD_=0
USE_MSBUILD_=1
endif

ifeq ($(USE_MSBUILD_),0)
all:	$(StroikaFrameworksLib)
else
all:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika-Frameworks:"
	@$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../Projects/$(ProjectPlatformSubdir)/Stroika-Frameworks-Led.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";
	@$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../Projects/$(ProjectPlatformSubdir)/Stroika-Frameworks-Service.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";
	@$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../Projects/$(ProjectPlatformSubdir)/Stroika-Frameworks-SystemPerformance.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";
	@$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../Projects/$(ProjectPlatformSubdir)/Stroika-Frameworks-WebServer.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";
	@$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../Projects/$(ProjectPlatformSubdir)/Stroika-Frameworks-WebService.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";
	@$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../Projects/$(ProjectPlatformSubdir)/Stroika-Frameworks.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";
endif


clean::
ifeq ($(CONFIGURATION),)
	@rm -rf $(StroikaRoot)IntermediateFiles/*/Frameworks
else
	@rm -rf $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Frameworks
endif

clobber::
ifeq (Unix,$(ProjectPlatformSubdir))
	@for i in $(SubDirs);\
	do\
	    $(MAKE) --directory $$i --no-print-directory --silent clobber;\
	done
	@rm -f $(StroikaFrameworksLib)
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
endif


Build:	all



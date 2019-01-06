export StroikaRoot?=$(shell realpath ../../../../)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir					=	$(StroikaRoot)Library/Sources/Stroika/Frameworks/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


VPATH		=	$(SrcDir) $(StroikaLibDir)


SubDirs=	\
		Modbus\
		NetworkMonitor\
		Service\
		SystemPerformance\
		WebServer\
		WebService\
		UPnP\

.PHONY:	$(SubDirs) all_objs_ all_objs_ all


.PHONY:			$(SubDirs)

$(SubDirs):
	@$(MAKE) --directory $@ -s


ALL_OBJS := $(foreach dir, $(SubDirs), $(shell $(MAKE) --directory $(dir) -s list-objs))

all-objs:
	@echo $(ALL_OBJS)


$(ObjDir)%.o : %.cpp
	@#suppress normal rule so just all_objs_ fires


$(ALL_OBJS): all_objs_

all_objs_:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika Frameworks Objs:";
	@$(MAKE) --no-print-directory -s $(SubDirs) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


$(StroikaFrameworksLib):	$(ALL_OBJS)
	@mkdir -p $(StroikaLibDir)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Creating `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh $@` ... "
	@rm -f $(StroikaFrameworksLib)
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
		echo "      $(AR) cr $(StroikaFrameworksLib) $(ALL_OBJS)";\
	fi
	@$(AR) cr $(StroikaFrameworksLib) $(ALL_OBJS)
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
		echo "      $(RANLIB) $(StroikaFrameworksLib)";\
	fi
	@$(RANLIB) $(StroikaFrameworksLib)


ifeq (Unix,$(ProjectPlatformSubdir))
all:	$(StroikaFrameworksLib)
else
all:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika-Frameworks:"
	@perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../Projects/$(ProjectPlatformSubdir)/Stroika-Frameworks-Led.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";
	@perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../Projects/$(ProjectPlatformSubdir)/Stroika-Frameworks-Service.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";
	@perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../Projects/$(ProjectPlatformSubdir)/Stroika-Frameworks-SystemPerformance.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";
	@perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../Projects/$(ProjectPlatformSubdir)/Stroika-Frameworks-WebServer.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";
	@perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../Projects/$(ProjectPlatformSubdir)/Stroika-Frameworks-WebService.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";
	@perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../Projects/$(ProjectPlatformSubdir)/Stroika-Frameworks.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";
endif


clean::
ifeq (Unix,$(ProjectPlatformSubdir))
	@for i in $(SubDirs);\
	do\
	     $(MAKE) --directory $$i --no-print-directory -s clean;\
	done
else
endif

clobber::
ifeq (Unix,$(ProjectPlatformSubdir))
	@for i in $(SubDirs);\
	do\
	    $(MAKE) --directory $$i --no-print-directory -s clobber;\
	done
	@rm -f $(StroikaFrameworksLib)
else
endif


Build:	all



export StroikaRoot?=$(shell realpath ../../../../)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk
endif

SrcDir					=	$(StroikaRoot)Library/Sources/Stroika/Foundation/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


.PHONY: all_objs_ all


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


ALL_OBJS := $(foreach dir, $(SubDirs), $(shell $(MAKE) --directory $(dir) -s list-objs CONFIGURATION=$(CONFIGURATION)))


all-objs:
	@echo $(ALL_OBJS)

$(ALL_OBJS):	all_objs_


$(ObjDir)%.o : %.cpp
	@#suppress normal rule so just all_objs_ fires


.PHONY:			$(SubDirs)

$(SubDirs):
	@$(MAKE) --directory $@ -s


all_objs_:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika Foundation Objs:";
	@$(MAKE) --no-print-directory -s $(SubDirs) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


$(StroikaFoundationLib):	$(ALL_OBJS)
	@mkdir -p $(StroikaLibDir)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Creating `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh $@` ... "
	@rm -f $(StroikaFoundationLib)
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
		echo "      $(AR) cr $(StroikaFoundationLib) $(ALL_OBJS)";\
	fi
	@$(AR) cr $(StroikaFoundationLib) $(ALL_OBJS)
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
		echo "      $(RANLIB) $(StroikaFoundationLib)";\
	fi
	@$(RANLIB) $(StroikaFoundationLib)



ifeq (Unix,$(ProjectPlatformSubdir))
all:	$(StroikaFoundationLib)
else
all:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika-Foundation:"
	@perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../Projects/$(ProjectPlatformSubdir)/Stroika-Foundation.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine.pl $(CONFIGURATION)` /target:Build";
endif


check:
	@$(StroikaRoot)ScriptsLib/CheckFileExists.sh $(StroikaFoundationLib)
	@$(ECHO) "[SUCCEEDED]"


clean::
ifeq (Unix,$(ProjectPlatformSubdir))
	@for i in $(SubDirs);\
	do\
	    $(MAKE) --directory $$i --no-print-directory -s clean CONFIGURATION=$(CONFIGURATION);\
	done
else
	@perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../Projects/$(ProjectPlatformSubdir)/Stroika-Foundation.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine.pl $(CONFIGURATION)` /target:Clean";
endif

clobber::
ifeq (Unix,$(ProjectPlatformSubdir))
	@rm -f $(StroikaFoundationLib)
else
	@perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../Projects/$(ProjectPlatformSubdir)/Stroika-Foundation.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine.pl $(CONFIGURATION)` /target:Clean";
endif

# not sure what that target for??? Lose
#Build:	all


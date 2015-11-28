.NOTPARALLEL:
.PHONY:	tests documentation all check clobber libraries
.FORCE:	check-tools
.FORCE:	apply-configurations


MAKE_INDENT_LEVEL?=$(MAKELEVEL)


help:
	@echo "Help for making Stroika:"
	@echo "Targets:"
	@echo "    all:                         -    Builds everything"
	@echo "    check:                       -    Checks everything was built properly"
	@echo "    clean:"
	@echo "    clobber:"
	@echo "    libraries:                   -    Builds Stroika foundation & frameworks, and any things it depends on (like third-party-libs)"
	@echo "    project-files:               -    Alias for project-files-visual-studio project-files-qt-creator"
	@echo "    project-files-visual-studio: -    Builds project files for visual studio.net"
	@echo "    project-files-qt-creator(*): -    Builds project project-files-qt-creator (also project-files-qt-creator-load project-files-qt-creator-save)"
	@echo "    tests:"
	@echo "    format-code:                 -    Run astyle on source code, and update it to conform to Stroika code formatting standards"
	@echo "    samples:"
	@echo "    documentation:"
	@echo "    third-party-libs:"
	@echo "    run-tests:                   -    [REMOTE=] - eg. REMOTE=lewis@localhost; [VALGRIND=1] to run with valgrind (EXTRA_VALGRIND_OPTIONS= can be used with valgrind)"
	@echo "    apply-configurations:        -    Create implied files / links for any configurations in the Configurations folder (forces a rebuild of configs)"
	@echo "    default-configuration:       -    Creates the default configuration in Configurations folder; [DEFAULT_CONFIGURATION_ARGS=--help])"
	@echo "    check-tools:                 -    Check the tools needed to build Stroika are installed."
	@echo "Special Variables:               -    Extra params you can pass to the make line that may help..."
	@echo "    ECHO_BUILD_LINES=1           -    Causes make lines to be echoed which can help makefile debugging"


all:		IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed libraries tools samples tests documentation
	@$(MAKE) --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)


check:
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory check CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Checking Stroika {$(CONFIGURATION)}:"
	@$(MAKE) --directory ThirdPartyLibs --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) MAKEFLAGS=
	@$(MAKE) --directory Library --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) MAKEFLAGS=
	@$(MAKE) --directory Tools --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) MAKEFLAGS=
	@$(MAKE) --directory Tests --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) MAKEFLAGS=
	@$(MAKE) --directory Samples --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) MAKEFLAGS=
endif


clean:
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory clean CONFIGURATION=$$i;\
	done
else
	@$(MAKE) --directory ThirdPartyLibs --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) MAKEFLAGS=
	@$(MAKE) --directory Library --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) MAKEFLAGS=
	@$(MAKE) --directory Tools --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) MAKEFLAGS=
	@$(MAKE) --directory Tests --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) MAKEFLAGS=
	@$(MAKE) --directory Samples --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) MAKEFLAGS=
endif


clobber:
	@ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Stroika Clobber..."
	@rm -rf IntermediateFiles/*
	@rm -rf Builds/*
	@#OK if no configuration or given configuration (handled in submake file)
	@$(MAKE) --directory ThirdPartyLibs --no-print-directory clobber CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)


documentation:
	@$(MAKE) --directory Documentation --no-print-directory all MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)


libraries:	IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed third-party-libs
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --directory Library --no-print-directory all CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@$(MAKE) --directory Library --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif


third-party-libs:	IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --directory ThirdPartyLibs --no-print-directory all CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@$(MAKE) --directory ThirdPartyLibs --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif


project-files:	project-files-visual-studio project-files-qt-creator


project-files-visual-studio:
	@$(MAKE) --directory Tests --no-print-directory MAKEFLAGS= project-files


project-files-qt-creator:	project-files-qt-creator-load


project-files-qt-creator-load:
	@echo -n "Loading qt-creator project files..."
	@for i in StroikaDevRoot.config StroikaDevRoot.creator StroikaDevRoot.files StroikaDevRoot.includes; do cp Library/Projects/QtCreator/$$i .; done;
	@echo "done"


project-files-qt-creator-save:
	@echo -n "Saving qt-creator project files..."
	@for i in StroikaDevRoot.config StroikaDevRoot.creator StroikaDevRoot.files StroikaDevRoot.includes; do cp $$i Library/Projects/QtCreator/ ; done;
	@echo "done"


tools:	libraries
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --directory Tools --no-print-directory all CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@$(MAKE) --directory Tools --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif


tests:	tools libraries
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --directory Tests --no-print-directory tests CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@$(MAKE) --directory Tests --no-print-directory tests CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif


samples:	tools libraries
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --directory Samples --no-print-directory all CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@$(MAKE) --directory Samples --no-print-directory samples CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif


run-tests:	tests
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --directory Tests --no-print-directory run-tests CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@$(MAKE) --directory Tests --no-print-directory run-tests CONFIGURATION=$(CONFIGURATION) MAKEFLAGS=  MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif


ASTYLE_ARGS=
ASTYLE_ARGS+=	--style=stroustrup
#ASTYLE_ARGS+=	--style=whitesmith
#ASTYLE_ARGS+=	--style=kr
#ASTYLE_ARGS+=	--style=java

ASTYLE_ARGS+=	--convert-tabs
ASTYLE_ARGS+=	--indent-namespaces
ASTYLE_ARGS+=	--indent-cases
ASTYLE_ARGS+=	--indent-switches
ASTYLE_ARGS+=	--indent-preprocessor
ASTYLE_ARGS+=	--pad-oper
ASTYLE_ARGS+=	--break-closing-brackets
ASTYLE_ARGS+=	--keep-one-line-blocks
ASTYLE_ARGS+=	--indent=spaces
ASTYLE_ARGS+=	--preserve-date

ASTYLE_ARGS+=	--align-pointer=type
ASTYLE_ARGS+=	--align-reference=type
ASTYLE_ARGS+=	--mode=c
ASTYLE_ARGS+=	--suffix=none

format-code:
	@echo Running Astyle...
	@for i in Library Samples Tests Tools;\
	do\
		astyle $(ASTYLE_ARGS) --recursive "$$i/*.h" "$$i/*.cpp" "$$i/*.inl" --formatted || true;\
	done


# useful internal check to make sure users dont run/build while missing key components that will
# just fail later
# but dont check if already checked
IntermediateFiles/TOOLS_CHECKED:
	@$(MAKE) check-tools --no-print-directory MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@mkdir -p IntermediateFiles
	@touch IntermediateFiles/TOOLS_CHECKED

# Force TOOLS_CHECKED test
check-tools:
	@#NOTE - we used to check for libtool, but thats only sometimes needed and we dont know if needed until after this rule (config based); its checked/warned about later
	@# no point in checking make ;-)
	@ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Checking for installed tools..."
	@echo -n "   ..." && sh -c "type sed"
	@echo -n "   ..." && sh -c "type wget"
	@echo -n "   ..." && sh -c "type perl"
	@echo -n "   ..." && sh -c "type tar"
	@echo -n "   ..." && sh -c "type patch"
ifneq (,$(findstring CYGWIN,$(shell uname)))
	@echo -n "   ..." && sh -c "type dos2unix"
	@echo -n "   ..." && sh -c "type unix2dos"
endif
	@echo "All Required Tools Present"




	

apply-configurations-if-needed:
	@#if no configurations, create default
ifeq ($(shell ScriptsLib/GetConfigurations.sh),)
	@$(MAKE) default-configuration --no-print-directory MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		if [ ! -e IntermediateFiles/$$i ] ; then\
			$(MAKE) --no-print-directory apply-configuration CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
		fi;\
	done


apply-configurations:
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory apply-configuration CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
	@touch IntermediateFiles/APPLIED_CONFIGURATIONS


apply-configuration:
	@ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Applying configuration $(CONFIGURATION)..."
	@mkdir -p "IntermediateFiles/$(CONFIGURATION)/"
	@perl ScriptsLib/ApplyConfiguration.pl $(CONFIGURATION)
	@echo "   ...Writing \"IntermediateFiles/$(CONFIGURATION)/Stroika-Current-Version.h\""
	@ScriptsLib/MakeVersionFile.sh STROIKA_VERSION IntermediateFiles/$(CONFIGURATION)/Stroika-Current-Version.h StroikaLibVersion


default-configuration:
	@ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo Making default configurations...
	@if [ `uname -o` = "Cygwin" ] ; then\
		./configure Debug-U-32 $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Debug-U-64 $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Release-DbgMemLeaks-U-32 $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Release-Logging-U-32 $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Release-Logging-U-64 $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Release-U-32 $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Release-U-64 $(DEFAULT_CONFIGURATION_ARGS);\
	else\
		./configure DefaultConfiguration $(DEFAULT_CONFIGURATION_ARGS);\
		#./configure Debug --assertions enable --GLIBCXX_DEBUG enable --trace2file enable $(DEFAULT_CONFIGURATION_ARGS);\
		#./configure Release --assertions disable --GLIBCXX_DEBUG enable --cpp-optimize-flag -03 $(DEFAULT_CONFIGURATION_ARGS);\
	fi
	

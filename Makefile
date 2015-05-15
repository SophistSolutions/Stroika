ACTIVE_CONFIGURATION 	?=	$(shell perl ScriptsLib/GetDefaultConfiguration.pl)
ProjectPlatformSubdir	=	$(shell perl ScriptsLib/PrintConfigurationVariable.pl $(ACTIVE_CONFIGURATION) ProjectPlatformSubdir)

ifeq (,$(findstring CYGWIN,$(shell uname)))
ALL_CONFIGURATIONS		=	$(ACTIVE_CONFIGURATION)
else
ALL_CONFIGURATIONS		=	Debug-U-32 Debug-U-64 Release-DbgMemLeaks-U-32 Release-Logging-U-32 Release-Logging-U-64 Release-U-32 Release-U-64
endif

.NOTPARALLEL: run-tests check apply-configurations third-party-libs
.PHONY:	tests documentation all check clobber libraries
.FORCE:	check-tools
.FORCE:	apply-configurations


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
	@$(MAKE) check


check:
	@$(MAKE) --directory ThirdPartyLibs --no-print-directory ACTIVE_CONFIGURATION=$(ACTIVE_CONFIGURATION) MAKEFLAGS= check
	@$(MAKE) --directory Library --no-print-directory ACTIVE_CONFIGURATION=$(ACTIVE_CONFIGURATION) MAKEFLAGS= check
	@(cd Tools && perl checkall.pl)
	@$(MAKE) --directory Samples --no-print-directory ACTIVE_CONFIGURATION=$(ACTIVE_CONFIGURATION) MAKEFLAGS= check
	@$(MAKE) --directory Tests --no-print-directory ACTIVE_CONFIGURATION=$(ACTIVE_CONFIGURATION) MAKEFLAGS= check


clean:
	@make --directory ThirdPartyLibs --no-print-directory ACTIVE_CONFIGURATION=$(ACTIVE_CONFIGURATION) MAKEFLAGS= clean
	@$(MAKE) --directory Library --no-print-directory ACTIVE_CONFIGURATION=$(ACTIVE_CONFIGURATION) MAKEFLAGS= clean
	@(cd Tools; perl buildall.pl clean)
	@$(MAKE) --directory Samples --no-print-directory ACTIVE_CONFIGURATION=$(ACTIVE_CONFIGURATION) MAKEFLAGS= clean
	@$(MAKE) --directory Tests --no-print-directory ACTIVE_CONFIGURATION=$(ACTIVE_CONFIGURATION) MAKEFLAGS= clean


clobber:
	@echo "Stroika Clobber..."
	@rm -rf IntermediateFiles
	@rm -rf Builds
	@$(MAKE) --directory ThirdPartyLibs --no-print-directory clobber


documentation:
	@cd Documentation/Doxygen && perl ./RunDoxygen.pl


libraries:	IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed third-party-libs
	@$(MAKE) --directory Library --no-print-directory ACTIVE_CONFIGURATION=$(ACTIVE_CONFIGURATION) all


third-party-libs:	IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed
	@$(MAKE) --directory ThirdPartyLibs --no-print-directory ACTIVE_CONFIGURATION=$(ACTIVE_CONFIGURATION) all


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
	@cd Tools; perl buildall.pl build


tests:	tools libraries
	@$(MAKE) --directory Tests --no-print-directory ACTIVE_CONFIGURATION=$(ACTIVE_CONFIGURATION) tests

samples:	tools libraries
	@$(MAKE) --directory Samples --no-print-directory ACTIVE_CONFIGURATION=$(ACTIVE_CONFIGURATION) samples

run-tests:	tests
	@$(MAKE) --directory Tests --no-print-directory run-tests ACTIVE_CONFIGURATION=$(ACTIVE_CONFIGURATION) MAKEFLAGS=

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
	@$(MAKE) check-tools --no-print-directory
	@mkdir -p IntermediateFiles
	@touch IntermediateFiles/TOOLS_CHECKED

# Force TOOLS_CHECKED test
check-tools:
	@#NOTE - we used to check for libtool, but thats only sometimes needed and we dont know if needed until after this rule (config based); its checked/warned about later
	@# no point in checking make ;-)
	@echo "Checking for installed tools..."
	@echo -n "   ..." && sh -c "type sed"
	@echo -n "   ..." && sh -c "type wget"
	@echo -n "   ..." && sh -c "type perl"
	@echo -n "   ..." && sh -c "type tar"
	@echo -n "   ..." && sh -c "type patch"
ifneq (,$(findstring CYGWIN,$(shell uname)))
	@echo -n "   ..." && sh -c "type dos2unix"
	@echo -n "   ..." && sh -c "type unix2dos"
endif
	@echo "   All Required Tools Present"





apply-configurations-if-needed:
	@test -e ConfigurationFiles/DefaultConfiguration.xml || $(MAKE) default-configuration --no-print-directory
	@test -e IntermediateFiles/APPLIED_CONFIGURATIONS || $(MAKE) apply-configurations --no-print-directory

apply-configurations:
	@for i in $(ALL_CONFIGURATIONS);\
	do\
		$(MAKE) --no-print-directory ACTIVE_CONFIGURATION=$$i apply-configuration;\
	done
	@touch IntermediateFiles/APPLIED_CONFIGURATIONS

apply-configuration:
	@echo "Applying configuration $(ACTIVE_CONFIGURATION)..."
ifneq (,$(findstring CYGWIN,$(shell uname)))
	@#tmphack
	@mkdir -p IntermediateFiles/DefaultConfiguration
endif
	@#todo - must enahnce ApplyConfiguration to support configuration arg
	@mkdir -p "IntermediateFiles/$(ACTIVE_CONFIGURATION)/"
	@perl ScriptsLib/ApplyConfiguration.pl
	@echo "   ...Writing \"IntermediateFiles/$(ACTIVE_CONFIGURATION)/Stroika-Current-Version.h\""
	@ScriptsLib/MakeVersionFile.sh STROIKA_VERSION IntermediateFiles/$(ACTIVE_CONFIGURATION)/Stroika-Current-Version.h StroikaLibVersion
	@echo "   ...Writing \"IntermediateFiles/$(ACTIVE_CONFIGURATION)/Stroika-Current-Configuration.h\""
ifneq (,$(findstring CYGWIN,$(shell uname)))
	@cp IntermediateFiles/DefaultConfiguration/Stroika-Current-Configuration.h IntermediateFiles/$(ACTIVE_CONFIGURATION)/Stroika-Current-Configuration.h
endif

default-configuration:
	@perl ScriptsLib/GenerateConfiguration.pl --default-for-platform $(DEFAULT_CONFIGURATION_ARGS)

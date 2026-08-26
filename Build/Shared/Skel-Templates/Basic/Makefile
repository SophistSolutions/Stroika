export TOP_ROOT:=$(abspath ./)/
StroikaRoot	:=	$(TOP_ROOT)ThirdPartyComponents/Stroika/StroikaRoot/

ifeq (,$(wildcard $(StroikaRoot)Makefile))
$(warning "submodules missing: perhaps you should run `git submodule update --init --recursive`")
endif

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk


#not parallel because submakefiles use parallelism, but generally best to sequence these top level requests. Like if you say
# make clobber all you don't want those to happen at the same time. And make libraries samples wouldn't really work since all the libraries
# have to be built before the samples etc...
.NOTPARALLEL:


#Handy shortcut
CONFIGURATION_TAGS?=$(TAGS)

APPLY_CONFIGS=$(or \
				$(CONFIGURATION), \
				$(if $(CONFIGURATION_TAGS), \
					$(shell $(StroikaRoot)Build/Scripts/GetConfigurations --config-tags "$(CONFIGURATION_TAGS)"),\
					$(if $(filter clobber, $(MAKECMDGOALS)),\
						$(shell $(StroikaRoot)Build/Scripts/GetConfigurations --all --quiet),\
						$(shell $(StroikaRoot)Build/Scripts/GetConfigurations --all-default)\
					)\
				)\
			)

all:
	@$(StroikaRoot)Build/Scripts/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "Building ${APP_NAME} all{$(CONFIGURATION)}:"
	@$(MAKE) -silent IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
ifeq ($(CONFIGURATION),)
	@#Cannot use APPLY_CONFIGS here because ConfigurationFiles may have changed and evaluated before here
	@for i in `$(StroikaRoot)Build/Scripts/GetConfigurations --config-tags "$(CONFIGURATION_TAGS)" --all-default` ; do\
		$(MAKE) --no-print-directory --silent all CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
else
	@$(MAKE) --directory=ThirdPartyComponents --no-print-directory all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(StroikaRoot)/Build/Scripts/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory=${APP_NAME} --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) all
endif


IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT:
ifeq ($(shell $(StroikaRoot)Build/Scripts/GetConfigurations --quiet),)
	@$(MAKE) -silent default-configurations MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


STROIKA_CONFIG_PARAMS_COMMON=

STROIKA_CONFIG_PARAMS_DEBUG=--apply-default-debug-flags
STROIKA_CONFIG_PARAMS_RELEASE=--apply-default-release-flags

.PHONY: default-configurations
default-configurations:
	@if [ ! -d ConfigurationFiles ] ; then $(MAKE) --silent build-root; fi
	@$(StroikaRoot)Build/Scripts/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) Configuring...
ifeq ($(DETECTED_HOST_OS), Darwin)
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaRoot) && ./configure Debug --build-by-default $(DETECTED_HOST_OS) --config-tag Unix $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_DEBUG));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaRoot) && ./configure Release --build-by-default $(DETECTED_HOST_OS) --config-tag Unix $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_RELEASE));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaRoot) && ./configure Release-Logging --config-tag Unix --trace2file enable $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_RELEASE));
else ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaRoot) && ./configure Debug --arch x86_64 --build-by-default never --config-tag Windows --config-tag x86_64 $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_DEBUG));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaRoot) && ./configure Release --arch x86_64 --build-by-default never --config-tag Windows --config-tag x86_64 $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_RELEASE));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaRoot) && ./configure Release-Logging --arch x86_64 --build-by-default $(DETECTED_HOST_OS) --config-tag Windows --config-tag x86_64 --trace2file enable $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_RELEASE));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaRoot) && ./configure Debug-x86 --arch x86 --build-by-default never --config-tag Windows --config-tag x86 $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_DEBUG));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaRoot) && ./configure Debug-x86_64 --arch x86_64 --build-by-default $(DETECTED_HOST_OS) --config-tag Windows --config-tag x86_64 $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_DEBUG));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaRoot) && ./configure Release-x86 --arch x86 --build-by-default $(DETECTED_HOST_OS) --config-tag Windows --config-tag x86 $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_RELEASE));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaRoot) && ./configure Release-x86_64 --arch x86_64 --build-by-default $(DETECTED_HOST_OS) --config-tag Windows --config-tag x86_64 $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_RELEASE));
else
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaRoot) && ./configure Debug --build-by-default $(DETECTED_HOST_OS) --config-tag Unix --only-if-has-compiler $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_DEBUG));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaRoot) && ./configure Release --build-by-default $(DETECTED_HOST_OS) --config-tag Unix --only-if-has-compiler $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_RELEASE));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaRoot) && ./configure Release-Logging --config-tag Unix --only-if-has-compiler --trace2file enable $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_RELEASE));
endif
ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	@$(MAKE) --silent Builds/__AUTOMATIC_MAKE_PROJECT_FILES__
endif
	@$(StroikaRoot)Build/Scripts/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "Applying configuration(s) to vscode:"
	@for i in `$(StroikaRoot)Build/Scripts/GetConfigurations --all` ; do\
		$(StroikaRoot)Build/Scripts/ApplyConfiguration --only-vscode $$i;\
	done

build-root:
	@$(StroikaRoot)Build/Scripts/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) Making BuildRoot...
	@export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaRoot) && ./Build/Scripts/MakeBuildRoot ../../../

apply-configurations-to-vscode:
	@$(StroikaRoot)Build/Scripts/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "Applying configuration(s) to vscode:"
	@for i in $(APPLY_CONFIGS) ; do\
		MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(StroikaRoot)Build/Scripts/ApplyConfiguration --only-vscode $$i;\
	done

list-configurations list-configuration-tags apply-configurations apply-configuration apply-configurations-if-needed reconfigure:
	@$(MAKE) --directory $(StroikaRoot) --silent CONFIGURATION_TAGS="$(CONFIGURATION_TAGS)" $@

project-files:
	@$(MAKE) --directory $(StroikaRoot) --silent CONFIGURATION_TAGS="$(CONFIGURATION_TAGS)" $@
	@#Workaround https://github.com/SophistSolutions/Stroika/issues/1075
	@cd Workspaces/VisualStudio.Net; rm -f Microsoft.Cpp.stroika.ConfigurationBased.props; $(StroikaRoot)/Build/Scripts/MakeSymbolicLink ../../ThirdPartyComponents/Stroika/StroikaRoot/Workspaces/VisualStudio.Net/Microsoft.Cpp.stroika.ConfigurationBased.props
	@cd Workspaces/VisualStudio.Net; rm -f Microsoft.Cpp.stroika.user.props; $(StroikaRoot)/Build/Scripts/MakeSymbolicLink ../../ThirdPartyComponents/Stroika/StroikaRoot/Workspaces/VisualStudio.Net/Microsoft.Cpp.stroika.user.props

distclean:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "${APP_NAME} $(call FUNCTION_CAPITALIZE_WORD,$@):"
ifneq ($(CONFIGURATION),)
	$(error "make distclean applies to all configurations - and deletes all configurations")
endif
	@rm -rf Builds ConfigurationFiles IntermediateFiles
	@$(MAKE) --no-print-directory clobber MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	-@$(MAKE) --no-print-directory --directory $(StroikaRoot) --silent $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))

clean clobber:
ifeq ($(CONFIGURATION),)
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "${APP_NAME} $(call FUNCTION_CAPITALIZE_WORD,$@):"
ifeq ($(CONFIGURATION_TAGS),)
	@if [ "$@"=="clobber" ] ; then \
		rm -rf IntermediateFiles/* Builds/*;\
	fi
	@if [ "$@" = "clean" ] ; then \
		rm -rf IntermediateFiles/*/${APP_NAME}*;\
	fi
	@# with no config specified, ${APP_NAME} NYI make clean/clobber (and not needed)
	@#$(MAKE) --silent --directory ${APP_NAME} $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));
	@$(MAKE) --silent --directory ThirdPartyComponents $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));
else
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	done
endif
else
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "${APP_NAME} $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)}:"
	@$(MAKE) --directory ${APP_NAME} --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif

ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
# This mechanism isn't strictly necessary, and slows builds slightly on windows. BUT, it can be very confusing the
# errors you get out of the box with visual studio projects without doing a make project-files, so include
# this to make the out of box experience installing Stroika a little more seemless.
# SEE https://github.com/SophistSolutions/Stroika/issues/1072
Builds/__AUTOMATIC_MAKE_PROJECT_FILES__:
	@$(MAKE) project-files
	@touch Builds/__AUTOMATIC_MAKE_PROJECT_FILES__
endif

update-submodules:
	git submodule update --init --recursive

STROIKA_COMMIT?=v3-Release
latest-submodules:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "Checkout Latest Stroika (STROIKA_COMMIT=${STROIKA_COMMIT})"
	@(cd $(StroikaRoot) && git checkout $(STROIKA_COMMIT) --quiet && git pull --quiet)

format-code:
	@$(MAKE) --directory=${APP_NAME} --no-print-directory format-code

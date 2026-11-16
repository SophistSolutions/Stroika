export TOP_ROOT:=$(abspath ./)/
StroikaSourceRoot	:=	$(TOP_ROOT)ThirdPartyComponents/Stroika/StroikaRoot/

ifeq (,$(wildcard $(StroikaSourceRoot)Makefile))
$(warning "submodules missing: perhaps you should run `git submodule update --init --recursive`")
endif

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk


#not parallel because submakefiles use parallelism, but generally best to sequence these top level requests. Like if you say
# make clobber all you don't want those to happen at the same time. And make libraries samples wouldn't really work since all the libraries
# have to be built before the samples etc...
.NOTPARALLEL:


#Handy shortcut
CONFIGURATION_TAGS?=$(TAGS)

APPLY_CONFIGS=$(or \
				$(CONFIGURATION), \
				$(if $(CONFIGURATION_TAGS), \
					$(shell $(StroikaDirectoryLayoutEnv) $(StroikaSourceRoot)Build/Scripts/GetConfigurations --config-tags "$(CONFIGURATION_TAGS)"),\
					$(if $(filter clobber, $(MAKECMDGOALS)),\
						$(shell $(StroikaDirectoryLayoutEnv) $(StroikaSourceRoot)Build/Scripts/GetConfigurations --all --quiet),\
						$(shell $(StroikaDirectoryLayoutEnv) $(StroikaSourceRoot)Build/Scripts/GetConfigurations --all-default)\
					)\
				)\
			)

all:
	@$(StroikaSourceRoot)Build/Scripts/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "Building ${APP_NAME} all{$(CONFIGURATION)}:"
	@$(MAKE) -silent $(StroikaIntermediateFilesRoot)ASSURE_DEFAULT_CONFIGURATIONS_BUILT MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
ifeq ($(CONFIGURATION),)
	@#Cannot use APPLY_CONFIGS here because ConfigurationFiles may have changed and evaluated before here
	@for i in `$(StroikaSourceRoot)Build/Scripts/GetConfigurations --config-tags "$(CONFIGURATION_TAGS)" --all-default` ; do\
		$(MAKE) --no-print-directory --silent all CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
else
	@$(MAKE) --directory=ThirdPartyComponents --no-print-directory all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(StroikaSourceRoot)/Build/Scripts/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory=${APP_NAME} --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) all
endif


$(StroikaIntermediateFilesRoot)ASSURE_DEFAULT_CONFIGURATIONS_BUILT:
ifeq ($(shell $(StroikaDirectoryLayoutEnv) $(StroikaSourceRoot)Build/Scripts/GetConfigurations --quiet),)
	@$(MAKE) -silent default-configurations MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


STROIKA_CONFIG_PARAMS_COMMON=

STROIKA_CONFIG_PARAMS_DEBUG=--apply-default-debug-flags
STROIKA_CONFIG_PARAMS_RELEASE=--apply-default-release-flags

.PHONY: default-configurations
default-configurations:
	@$(StroikaSourceRoot)Build/Scripts/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) Configuring...
ifeq ($(DETECTED_HOST_OS), Darwin)
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaSourceRoot) && ./configure Debug --build-by-default $(DETECTED_HOST_OS) --config-tag Unix $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_DEBUG));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaSourceRoot) && ./configure Release --build-by-default $(DETECTED_HOST_OS) --config-tag Unix $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_RELEASE));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaSourceRoot) && ./configure Release-Logging --config-tag Unix --trace2file enable $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_RELEASE));
else ifneq ($(filter Windows,$(StroikaHostOSNames)),)
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaSourceRoot) && ./configure Debug --arch x86_64 --build-by-default never --config-tag Windows --config-tag x86_64 $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_DEBUG));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaSourceRoot) && ./configure Release --arch x86_64 --build-by-default never --config-tag Windows --config-tag x86_64 $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_RELEASE));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaSourceRoot) && ./configure Release-Logging --arch x86_64 --build-by-default Windows --config-tag Windows --config-tag x86_64 --trace2file enable $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_RELEASE));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaSourceRoot) && ./configure Debug-x86 --arch x86 --build-by-default never --config-tag Windows --config-tag x86 $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_DEBUG));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaSourceRoot) && ./configure Debug-x86_64 --arch x86_64 --build-by-default Windows --config-tag Windows --config-tag x86_64 $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_DEBUG));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaSourceRoot) && ./configure Release-x86 --arch x86 --build-by-default Windows --config-tag Windows --config-tag x86 $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_RELEASE));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaSourceRoot) && ./configure Release-x86_64 --arch x86_64 --build-by-default Windows --config-tag Windows --config-tag x86_64 $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_RELEASE));
else
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaSourceRoot) && ./configure Debug --build-by-default $(DETECTED_HOST_OS) --config-tag Unix --only-if-has-compiler $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_DEBUG));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaSourceRoot) && ./configure Release --build-by-default $(DETECTED_HOST_OS) --config-tag Unix --only-if-has-compiler $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_RELEASE));
	@(export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && cd $(StroikaSourceRoot) && ./configure Release-Logging --config-tag Unix --only-if-has-compiler --trace2file enable $(STROIKA_CONFIG_PARAMS_COMMON) $(STROIKA_CONFIG_PARAMS_RELEASE));
endif
ifneq ($(filter Windows,$(StroikaHostOSNames)),)
	@$(MAKE) --silent $(StroikaBuildsRoot)__AUTOMATIC_MAKE_PROJECT_FILES__
endif
	@$(StroikaSourceRoot)Build/Scripts/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "Applying configuration(s) to vscode:"
	@for i in `$(StroikaSourceRoot)Build/Scripts/GetConfigurations --all` ; do\
		$(StroikaSourceRoot)Build/Scripts/ApplyConfiguration --only-vscode $$i;\
	done

apply-configurations-to-vscode:
	@$(StroikaSourceRoot)Build/Scripts/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "Applying configuration(s) to vscode:"
	@for i in $(APPLY_CONFIGS) ; do\
		MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(StroikaSourceRoot)Build/Scripts/ApplyConfiguration --only-vscode $$i;\
	done

list-configurations list-configuration-tags apply-configurations apply-configuration apply-configurations-if-needed reconfigure:
	@$(MAKE) --directory $(StroikaSourceRoot) --silent CONFIGURATION_TAGS="$(CONFIGURATION_TAGS)" $@

project-files:
	@$(MAKE) --directory $(StroikaSourceRoot) --silent CONFIGURATION_TAGS="$(CONFIGURATION_TAGS)" $@
	@#Workaround https://github.com/SophistSolutions/Stroika/issues/1075
	@cd Workspaces/VisualStudio.Net; rm -f Microsoft.Cpp.stroika.ConfigurationBased.props; $(StroikaSourceRoot)/Build/Scripts/MakeFileLink ../../ThirdPartyComponents/Stroika/StroikaRoot/Workspaces/VisualStudio.Net/Microsoft.Cpp.stroika.ConfigurationBased.props
	@cd Workspaces/VisualStudio.Net; rm -f Microsoft.Cpp.stroika.user.props; $(StroikaSourceRoot)/Build/Scripts/MakeFileLink ../../ThirdPartyComponents/Stroika/StroikaRoot/Workspaces/VisualStudio.Net/Microsoft.Cpp.stroika.user.props

distclean:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "${APP_NAME} $(call FUNCTION_CAPITALIZE_WORD,$@):"
ifneq ($(CONFIGURATION),)
	$(error "make distclean applies to all configurations - and deletes all configurations")
endif
	@$(MAKE) --no-print-directory clobber MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	-@$(MAKE) --no-print-directory --directory $(StroikaSourceRoot) --silent $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@# last: the makes above need the build root; ConfigurationFiles/ is where the configurations were before Stroika v3.0d26
	@rm -rf $(patsubst %/,%,$(StroikaBuildsRoot) $(StroikaIntermediateFilesRoot)) $(TOP_ROOT)ConfigurationFiles

clean clobber:
ifeq ($(CONFIGURATION),)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "${APP_NAME} $(call FUNCTION_CAPITALIZE_WORD,$@):"
ifeq ($(CONFIGURATION_TAGS),)
	@if [ "$@" = "clobber" ] ; then \
		$(StroikaClobberIntermediateFiles); rm -rf $(StroikaBuildsRoot)*;\
	fi
	@if [ "$@" = "clean" ] ; then \
		rm -rf $(StroikaIntermediateFilesRoot)*/${APP_NAME}*;\
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
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "${APP_NAME} $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)}:"
	@$(MAKE) --directory ${APP_NAME} --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif

ifneq ($(filter Windows,$(StroikaHostOSNames)),)
# This mechanism isn't strictly necessary, and slows builds slightly on windows. BUT, it can be very confusing the
# errors you get out of the box with visual studio projects without doing a make project-files, so include
# this to make the out of box experience installing Stroika a little more seemless.
# SEE https://github.com/SophistSolutions/Stroika/issues/1072
$(StroikaBuildsRoot)__AUTOMATIC_MAKE_PROJECT_FILES__:
	@$(MAKE) project-files
	@touch $(StroikaBuildsRoot)__AUTOMATIC_MAKE_PROJECT_FILES__
endif

update-submodules:
	git submodule update --init --recursive

STROIKA_COMMIT?=v3-Release
latest-submodules:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "Checkout Latest Stroika (STROIKA_COMMIT=${STROIKA_COMMIT})"
	@(cd $(StroikaSourceRoot) && git checkout $(STROIKA_COMMIT) --quiet && git pull --quiet)

format-code:
	@$(MAKE) --directory=${APP_NAME} --no-print-directory format-code

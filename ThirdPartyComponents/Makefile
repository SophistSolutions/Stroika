include ../ScriptsLib/Makefile-Common.mk
ifneq ($(CONFIGURATION),)
-include ../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk
endif

SHELL=/bin/bash


.PHONY:	all clean clobber check

MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ../ScriptsLib/GetDefaultShellVariable.sh ECHO)



#Handy shortcut
CONFIGURATION_TAGS?=$(TAGS)


APPLY_CONFIGS=$(or $(CONFIGURATION), $(shell ScriptsLib/GetConfigurations --config-tags "$(CONFIGURATION_TAGS)"))



ifneq ($(CONFIGURATION),)

ProjectPlatformSubdir	:=	$(shell ../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" ProjectPlatformSubdir)

ifeq ($(shell ../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" qFeatureFlag_ActivePerl), use)
	USE_ActivePerl=1
endif
ifeq ($(shell ../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" qFeatureFlag_boost), use)
	USE_boost=1
endif
ifeq ($(shell ../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" qFeatureFlag_PrivateOverrideOfCMake), use)
	USE_cmake=1
endif
ifeq ($(shell ../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" qFeatureFlag_LibCurl), use)
	USE_curl=1
endif
ifeq ($(shell ../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" qFeatureFlag_LZMA), use)
	USE_lzma=1
endif
ifeq ($(shell ../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" qFeatureFlag_OpenSSL), use)
	USE_openssl=1
endif
ifeq ($(shell ../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" qFeatureFlag_sqlite), use)
	USE_sqlite=1
endif
ifeq ($(shell ../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" qFeatureFlag_Xerces), use)
	USE_Xerces=1
endif
ifeq ($(shell ../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" qFeatureFlag_ZLib), use)
	USE_zlib=1
endif



SubDirs=
ifeq ($(USE_ActivePerl), 1)
	SubDirs+= ActivePerl
endif
ifeq ($(USE_boost), 1)
	SubDirs+= boost
endif
ifeq ($(USE_cmake), 1)
	SubDirs+= cmake
endif
ifeq ($(USE_curl), 1)
	SubDirs+= curl
endif
ifeq ($(USE_lzma), 1)
	SubDirs+= lzma
endif
ifeq ($(USE_openssl), 1)
	SubDirs+= openssl
endif
ifeq ($(USE_sqlite), 1)
	SubDirs+= sqlite
endif
ifeq ($(USE_Xerces), 1)
	SubDirs+= Xerces
endif
ifeq ($(USE_zlib), 1)
	SubDirs+= zlib
endif


##WINDOWS ONLY
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq ($(USE_openssl), 1)
openssl:	cmake
openssl:	ActivePerl
endif
endif


ifeq ($(USE_boost), 1)
ifeq ($(USE_zlib), 1)
boost:		zlib
endif
endif

ifeq ($(USE_curl), 1)
ifeq ($(USE_openssl), 1)
curl:		openssl
endif
endif

ifeq ($(USE_openssl), 1)
ifeq ($(USE_zlib), 1)
openssl:	zlib
endif
endif

###NOTE - because we set network:BOOL:OFF - Xerces doesn't depend on CURL (but we could make it so if we wanted, but then update dependencies
### in ThirdPartyLibs/Makefile
#ifeq ($(USE_Xerces), 1)
#ifeq ($(USE_curl), 1)
#Xerces:		curl
#endif
#endif



.PHONY: $(SubDirs)

$(SubDirs):
	@$(MAKE) --directory $@ --silent all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


all:
ifeq ($(CONFIGURATION),)
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i;\
	done
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika ThirdPartyComponents Build {$(CONFIGURATION)}:"
ifeq ($(SubDirs),)
	@../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "none"
else
	@$(MAKE) --silent $(SubDirs)
endif
endif


check-prerequisite-tools:
ifeq ($(CONFIGURATION),)
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika ThirdPartyComponents Checking-Prerequisite Tools:"
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory check-prerequisite-tools CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	done
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Checking-Prerequisite Tools Stroika ThirdPartyComponents {$(CONFIGURATION)}:"
ifeq ($(USE_boost), 1)
	@$(MAKE) --directory boost --silent check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(USE_openssl), 1)
	@$(MAKE) --directory openssl --silent check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(USE_curl), 1)
	@$(MAKE) --directory curl --silent check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(USE_lzma), 1)
	@$(MAKE) --directory lzma --silent check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(USE_sqlite), 1)
	@$(MAKE) --directory sqlite --silent check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(USE_Xerces), 1)
	@$(MAKE) --directory Xerces --silent check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(USE_zlib), 1)
	@$(MAKE) --directory zlib --silent check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
endif




check:
ifeq ($(CONFIGURATION),)
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	done
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika ThirdPartyComponents Check {$(CONFIGURATION)}:"
ifeq ($(USE_boost), 1)
	@$(MAKE) --directory boost --no-print-directory $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(USE_curl), 1)
	@$(MAKE) --directory curl --no-print-directory $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(USE_lzma), 1)
	@$(MAKE) --directory lzma --no-print-directory $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(USE_openssl), 1)
	@$(MAKE) --directory openssl --no-print-directory $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(USE_sqlite), 1)
	@$(MAKE) --directory sqlite --no-print-directory $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(USE_Xerces), 1)
	@$(MAKE) --directory Xerces --no-print-directory $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(USE_zlib), 1)
	@$(MAKE) --directory zlib --no-print-directory $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
endif


clean clobber:
ifeq ($(CONFIGURATION),)
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL)  "Stroika/ThirdPartyComponents $(call FUNCTION_CAPITALIZE_WORD,$@):"
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL)  "Stroika/ThirdPartyComponents $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)}:"
endif
	@$(MAKE) --directory ActivePerl --silent $@
	@$(MAKE) --directory boost --silent $@
	@$(MAKE) --directory cmake --silent $@
	@$(MAKE) --directory curl --silent $@
	@$(MAKE) --directory lzma --silent $@
	@$(MAKE) --directory openssl --silent $@
	@$(MAKE) --directory sqlite --silent $@
	@$(MAKE) --directory Xerces --silent $@
	@$(MAKE) --directory zlib --silent $@

ifneq ($(CONFIGURATION),)
-include ../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk
endif

SHELL=/bin/bash


.PHONY:	all clean clobber check
.NOTPARALLEL:

MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ../ScriptsLib/GetDefaultShellVariable.sh ECHO)


all:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i;\
	done
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika ThirdPartyComponents {$(CONFIGURATION)}:"
ifeq ($(shell perl ../ScriptsLib/GetConfigurationParameter $(CONFIGURATION) qFeatureFlag_ActivePerl), use)
	@$(MAKE) --directory ActivePerl --no-print-directory all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(shell perl ../ScriptsLib/GetConfigurationParameter $(CONFIGURATION) qFeatureFlag_PrivateOverrideOfCMake), use)
	@$(MAKE) --directory cmake --no-print-directory all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(shell perl ../ScriptsLib/GetConfigurationParameter $(CONFIGURATION) qFeatureFlag_ZLib), use)
	@$(MAKE) --directory zlib --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(shell perl ../ScriptsLib/GetConfigurationParameter $(CONFIGURATION) qFeatureFlag_boost), use)
	@$(MAKE) --directory boost --no-print-directory all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(shell perl ../ScriptsLib/GetConfigurationParameter $(CONFIGURATION) qFeatureFlag_OpenSSL), use)
	@$(MAKE) --directory openssl --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(shell perl ../ScriptsLib/GetConfigurationParameter $(CONFIGURATION) qFeatureFlag_LibCurl), use)
	@$(MAKE) --directory curl --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(shell perl ../ScriptsLib/GetConfigurationParameter $(CONFIGURATION) qFeatureFlag_LZMA), use)
	@$(MAKE) --directory lzma --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(shell perl ../ScriptsLib/GetConfigurationParameter $(CONFIGURATION) qFeatureFlag_Xerces), use)
	@$(MAKE) --directory Xerces --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(shell perl ../ScriptsLib/GetConfigurationParameter $(CONFIGURATION) qFeatureFlag_sqlite), use)
	@$(MAKE) --directory sqlite --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
endif


check-prerequisite-tools:
ifeq ($(CONFIGURATION),)
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Checking-Prerequisite Tools Stroika ThirdPartyComponents:"
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory check-prerequisite-tools CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	done
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Checking-Prerequisite Tools Stroika ThirdPartyComponents {$(CONFIGURATION)}:"
ifeq ($(shell perl ../ScriptsLib/GetConfigurationParameter $(CONFIGURATION) qFeatureFlag_boost), use)
	@$(MAKE) --directory boost --silent check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_OpenSSL), 1)
	@$(MAKE) --directory openssl --silent check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_libcurl), 1)
	@$(MAKE) --directory curl --silent check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_LZMA), 1)
	@$(MAKE) --directory lzma --silent check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_sqlite), 1)
	@$(MAKE) --directory sqlite --silent check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_Xerces), 1)
	@$(MAKE) --directory Xerces --silent check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_ZLib), 1)
	@$(MAKE) --directory zlib --silent check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
endif




check:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	done
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Checking Stroika ThirdPartyComponents {$(CONFIGURATION)}:"
ifeq ($(shell perl ../ScriptsLib/GetConfigurationParameter $(CONFIGURATION) qFeatureFlag_boost), use)
	@$(MAKE) --directory boost --no-print-directory $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_libcurl), 1)
	@$(MAKE) --directory curl --no-print-directory $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_LZMA), 1)
	@$(MAKE) --directory lzma --no-print-directory $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_OpenSSL), 1)
	@$(MAKE) --directory openssl --no-print-directory $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_sqlite), 1)
	@$(MAKE) --directory sqlite --no-print-directory $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_Xerces), 1)
	@$(MAKE) --directory Xerces --no-print-directory $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_ZLib), 1)
	@$(MAKE) --directory zlib --no-print-directory $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
endif


clean:
	@#ok to call with NO CONFIG (does all)
ifeq ($(CONFIGURATION),)
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Cleaning Stroika ThirdPartyComponents:"
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Cleaning Stroika ThirdPartyComponents{$(CONFIGURATION)}:"
endif
	@$(MAKE) --directory ActivePerl --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory boost --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory cmake --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory curl --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory lzma --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory openssl --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory sqlite --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory Xerces --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory zlib --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(ECHO) done


clobber:
	@#ok to call with NO CONFIG (does all)
ifeq ($(CONFIGURATION),)
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Clobbering Stroika ThirdPartyComponents ... "
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Clobbering Stroika ThirdPartyComponents {$(CONFIGURATION)}:"
endif
	@$(MAKE) --directory ActivePerl --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory boost --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory cmake --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory curl --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory lzma --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory openssl --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory sqlite --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory Xerces --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory zlib --silent $@ CONFIGURATION=$(CONFIGURATION)
	@$(ECHO) "done"

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
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika {$(CONFIGURATION)} ThirdPartyComponents:"
ifeq ($(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) qFeatureFlag_ActivePerl), use)
	@$(MAKE) --directory ActivePerl --no-print-directory all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) qFeatureFlag_ZLib), use)
	@$(MAKE) --directory zlib --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) qFeatureFlag_OpenSSL), use)
	@$(MAKE) --directory openssl --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) qFeatureFlag_LibCurl), use)
	@$(MAKE) --directory curl --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) qFeatureFlag_LZMA), use)
	@$(MAKE) --directory lzma --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) qFeatureFlag_Xerces), use)
	@$(MAKE) --directory Xerces --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) qFeatureFlag_sqlite), use)
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
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Checking-Prerequisite Tools Stroika {$(CONFIGURATION)} ThirdPartyComponents:"
ifeq ($(qBuildThirdPartyComponents_ZLib), 1)
	@$(MAKE) --directory zlib --no-print-directory check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_OpenSSL), 1)
	@$(MAKE) --directory openssl --no-print-directory check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_libcurl), 1)
	@$(MAKE) --directory curl --no-print-directory check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_LZMA), 1)
	@$(MAKE) --directory lzma --no-print-directory check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_Xerces), 1)
	@$(MAKE) --directory Xerces --no-print-directory check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_sqlite), 1)
	@$(MAKE) --directory sqlite --no-print-directory check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
endif




check:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory check CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	done
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Checking Stroika {$(CONFIGURATION)} ThirdPartyComponents:"
ifeq ($(qBuildThirdPartyComponents_ZLib), 1)
	@$(MAKE) --directory zlib --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_OpenSSL), 1)
	@$(MAKE) --directory openssl --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_libcurl), 1)
	@$(MAKE) --directory curl --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_LZMA), 1)
	@$(MAKE) --directory lzma --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_Xerces), 1)
	@$(MAKE) --directory Xerces --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_sqlite), 1)
	@$(MAKE) --directory sqlite --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
endif


clean:
ifeq ($(CONFIGURATION),)
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Cleaning Stroika ThirdPartyComponents..."
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Cleaning Stroika {$(CONFIGURATION)} ThirdPartyComponents..."
endif
	@#ok to call with NO CONFIG (does all)
	@$(MAKE) --directory zlib --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory openssl --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory curl --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory lzma --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory Xerces --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory sqlite --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(ECHO) done


clobber:
ifeq ($(CONFIGURATION),)
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Clobbering Stroika ThirdPartyComponents..."
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Clobbering Stroika {$(CONFIGURATION)} ThirdPartyComponents..."
endif
	@#ok to call with NO CONFIG (does all)
	@$(MAKE) --directory zlib --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory openssl --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory curl --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory lzma --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory Xerces --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory sqlite --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(ECHO) "done"

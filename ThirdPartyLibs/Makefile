CONFIGURATION 			?=	$(shell perl ../ScriptsLib/GetDefaultConfiguration.pl)

-include ../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

.PHONY:	all clean clobber check
.NOTPARALLEL:


all:
	@echo "Building Stroika ThirdPartyLibs..."
ifeq ($(qBuildThirdPartyProducts_ZLib), 1)
	@$(MAKE) --directory zlib --no-print-directory CONFIGURATION=$(CONFIGURATION) all
endif
ifeq ($(qBuildThirdPartyProducts_OpenSSL), 1)
	@$(MAKE) --directory openssl --no-print-directory CONFIGURATION=$(CONFIGURATION) all
endif
ifeq ($(qBuildThirdPartyProducts_libcurl), 1)
	@$(MAKE) --directory curl --no-print-directory CONFIGURATION=$(CONFIGURATION) all
endif
ifeq ($(qBuildThirdPartyProducts_LZMA), 1)
	@$(MAKE) --directory lzma --no-print-directory CONFIGURATION=$(CONFIGURATION) all
endif
ifeq ($(qBuildThirdPartyProducts_Xerces), 1)
	@$(MAKE) --directory Xerces --no-print-directory CONFIGURATION=$(CONFIGURATION) all
endif
	@echo "Building Stroika ThirdPartyLibs...done"

clean:
	@$(MAKE) --directory zlib --no-print-directory CONFIGURATION=$(CONFIGURATION) clean
	@$(MAKE) --directory openssl --no-print-directory CONFIGURATION=$(CONFIGURATION) clean
	@$(MAKE) --directory curl --no-print-directory CONFIGURATION=$(CONFIGURATION) clean
	@$(MAKE) --directory lzma --no-print-directory CONFIGURATION=$(CONFIGURATION) clean
	@$(MAKE) --directory Xerces --no-print-directory CONFIGURATION=$(CONFIGURATION) clean

check:
	@echo "Checking Stroika ThirdPartyLibs..."
ifeq ($(qBuildThirdPartyProducts_ZLib), 1)
	@$(MAKE) --directory zlib $(MAKEFLAGS) --no-print-directory CONFIGURATION=$(CONFIGURATION) check
endif
ifeq ($(qBuildThirdPartyProducts_OpenSSL), 1)
	@$(MAKE) --directory openssl --no-print-directory CONFIGURATION=$(CONFIGURATION) check
endif
ifeq ($(qBuildThirdPartyProducts_libcurl), 1)
	@$(MAKE) --directory curl $(MAKEFLAGS) --no-print-directory CONFIGURATION=$(CONFIGURATION) check
endif
ifeq ($(qBuildThirdPartyProducts_LZMA), 1)
	@$(MAKE) --directory lzma --no-print-directory CONFIGURATION=$(CONFIGURATION) check
endif
ifeq ($(qBuildThirdPartyProducts_Xerces), 1)
	@$(MAKE) --directory Xerces --no-print-directory CONFIGURATION=$(CONFIGURATION) check
endif

clobber:
	@echo -n "Stroika ThirdPartyLibs clobber..."
	@$(MAKE) --directory zlib --no-print-directory CONFIGURATION=$(CONFIGURATION) clobber
	@$(MAKE) --directory openssl --no-print-directory CONFIGURATION=$(CONFIGURATION) clobber
	@$(MAKE) --directory curl --no-print-directory CONFIGURATION=$(CONFIGURATION) clobber
	@$(MAKE) --directory lzma --no-print-directory CONFIGURATION=$(CONFIGURATION) clobber
	@$(MAKE) --directory Xerces --no-print-directory CONFIGURATION=$(CONFIGURATION) clobber
	@echo "   ...done"

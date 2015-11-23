ifeq ($(CONFIGURATION),)
$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif

-include ../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

.PHONY:	all clean clobber check
.NOTPARALLEL:


all:
	@echo "Building Stroika {$(CONFIGURATION)} ThirdPartyLibs..."
ifeq ($(qBuildThirdPartyProducts_ZLib), 1)
	@$(MAKE) --directory zlib --no-print-directory all CONFIGURATION=$(CONFIGURATION)
endif
ifeq ($(qBuildThirdPartyProducts_OpenSSL), 1)
	@$(MAKE) --directory openssl --no-print-directory all CONFIGURATION=$(CONFIGURATION)
endif
ifeq ($(qBuildThirdPartyProducts_libcurl), 1)
	@$(MAKE) --directory curl --no-print-directory all CONFIGURATION=$(CONFIGURATION)
endif
ifeq ($(qBuildThirdPartyProducts_LZMA), 1)
	@$(MAKE) --directory lzma --no-print-directory all CONFIGURATION=$(CONFIGURATION)
endif
ifeq ($(qBuildThirdPartyProducts_Xerces), 1)
	@$(MAKE) --directory Xerces --no-print-directory all CONFIGURATION=$(CONFIGURATION)
endif
	@echo "Building Stroika {$(CONFIGURATION)} ThirdPartyLibs...done"

clean:
	@$(MAKE) --directory zlib --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory openssl --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory curl --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory lzma --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory Xerces --no-print-directory clean CONFIGURATION=$(CONFIGURATION)

check:
	@echo "Checking Stroika {$(CONFIGURATION)} ThirdPartyLibs..."
ifeq ($(qBuildThirdPartyProducts_ZLib), 1)
	@$(MAKE) --directory zlib $(MAKEFLAGS) --no-print-directory check CONFIGURATION=$(CONFIGURATION)
endif
ifeq ($(qBuildThirdPartyProducts_OpenSSL), 1)
	@$(MAKE) --directory openssl --no-print-directory check CONFIGURATION=$(CONFIGURATION)
endif
ifeq ($(qBuildThirdPartyProducts_libcurl), 1)
	@$(MAKE) --directory curl $(MAKEFLAGS) --no-print-directory check CONFIGURATION=$(CONFIGURATION)
endif
ifeq ($(qBuildThirdPartyProducts_LZMA), 1)
	@$(MAKE) --directory lzma --no-print-directory check CONFIGURATION=$(CONFIGURATION)
endif
ifeq ($(qBuildThirdPartyProducts_Xerces), 1)
	@$(MAKE) --directory Xerces --no-print-directory check CONFIGURATION=$(CONFIGURATION)
endif

clobber:
	@echo -n "Stroika {$(CONFIGURATION)} ThirdPartyLibs clobber..."
	@$(MAKE) --directory zlib --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory openssl --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory curl --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory lzma --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory Xerces --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@echo "   ...done"

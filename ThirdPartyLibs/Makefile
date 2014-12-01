-include ../IntermediateFiles/DefaultConfiguration/Library/Configuration.mk

.PHONY:	all clean clobber check

all:
ifeq ($(qBuildThirdPartyProducts_OpenSSL), 1)
	@$(MAKE) --directory openssl --no-print-directory all
endif
ifeq ($(qBuildThirdPartyProducts_libcurl), 1)
	@$(MAKE) --directory curl --no-print-directory all
endif
ifeq ($(qBuildThirdPartyProducts_Xerces), 1)
	@$(MAKE) --directory Xerces --no-print-directory all
endif
ifeq ($(qBuildThirdPartyProducts_zlib), 1)
	@$(MAKE) --directory zlib --no-print-directory all
endif

clean:
	@$(MAKE) --directory openssl --no-print-directory clean
	@$(MAKE) --directory zlib --no-print-directory clean
	@$(MAKE) --directory Xerces --no-print-directory clean
	@$(MAKE) --directory curl --no-print-directory clean

check:
	@echo Checking ThirdPartyLibs...
ifeq ($(qBuildThirdPartyProducts_OpenSSL), 1)
	@$(MAKE) --directory openssl --no-print-directory check
endif
ifeq ($(qBuildThirdPartyProducts_libcurl), 1)
	@$(MAKE) --directory curl $(MAKEFLAGS) --no-print-directory check
endif
ifeq ($(qBuildThirdPartyProducts_Xerces), 1)
	@$(MAKE) --directory Xerces --no-print-directory check
endif
ifeq ($(qBuildThirdPartyProducts_zlib), 1)
	@$(MAKE) --directory zlib $(MAKEFLAGS) --no-print-directory check
endif

clobber:
	@echo "ThirdPartyLibs clobber..."
	@$(MAKE) --directory openssl --no-print-directory clobber
	@$(MAKE) --directory curl --no-print-directory clobber
	@$(MAKE) --directory Xerces --no-print-directory clobber
	@$(MAKE) --directory zlib --no-print-directory clobber
	@echo "ThirdPartyLibs clobber...done"

-include ../IntermediateFiles/DefaultConfiguration/Library/Configuration.mk

.PHONY:	all clean clobber check

all:
	@(cd doxygen; perl buildall.pl Build)
ifeq ($(qBuildThirdPartyProducts_OpenSSL), 1)
	@$(MAKE) --directory openssl all
endif
ifeq ($(qBuildThirdPartyProducts_libcurl), 1)
	@$(MAKE) --directory curl all
endif
ifeq ($(qBuildThirdPartyProducts_Xerces), 1)
	@$(MAKE) --directory Xerces all
endif
ifeq ($(qBuildThirdPartyProducts_zlib), 1)
	@$(MAKE) --directory zlib all
endif

clean:
	@(cd doxygen; perl buildall.pl clean)
	@$(MAKE) --directory openssl --no-print-directory clean
	@$(MAKE) --directory zlib --no-print-directory clean
	@$(MAKE) --directory Xerces --no-print-directory clean
	@$(MAKE) --directory curl --no-print-directory clean

check:
	@echo Checking ThirdPartyLibs...
	@(cd doxygen; perl checkall.pl)
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
	@echo "ThirdPartyProducts clobber..."
	@(cd doxygen; perl buildall.pl clobber)
	@$(MAKE) --directory openssl --no-print-directory clobber
	@$(MAKE) --directory curl --no-print-directory clobber
	@$(MAKE) --directory Xerces --no-print-directory clobber
	@$(MAKE) --directory zlib --no-print-directory clobber
	@echo "ThirdPartyProducts clobber...done"

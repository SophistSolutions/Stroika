-include ../IntermediateFiles/DefaultConfiguration/Library/Configuration.mk

.PHONY:	all clean clobber check

all:
	@(cd doxygen; perl buildall.pl Build)
ifeq ($(qBuildThirdPartyProducts_OpenSSL), 1)
	@make --directory openssl all
endif
ifeq ($(qBuildThirdPartyProducts_libcurl), 1)
	@make --directory curl all
endif
ifeq ($(qBuildThirdPartyProducts_Xerces), 1)
	@make --directory Xerces all
endif
ifeq ($(qBuildThirdPartyProducts_zlib), 1)
	@make --directory zlib all
endif

clean:
	@(cd doxygen; perl buildall.pl clean)
	@make --directory openssl --no-print-directory clean
	@make --directory zlib --no-print-directory clean
	@make --directory Xerces --no-print-directory clean
	@make --directory curl --no-print-directory clean

check:
	@echo Checking ThirdPartyLibs...
	@(cd doxygen; perl checkall.pl)
ifeq ($(qBuildThirdPartyProducts_OpenSSL), 1)
	@make --directory openssl --no-print-directory check
endif
ifeq ($(qBuildThirdPartyProducts_libcurl), 1)
	@make --directory curl $(MAKEFLAGS) --no-print-directory check
endif
ifeq ($(qBuildThirdPartyProducts_Xerces), 1)
	@make --directory Xerces --no-print-directory check
endif
ifeq ($(qBuildThirdPartyProducts_zlib), 1)
	@make --directory zlib $(MAKEFLAGS) --no-print-directory check
endif

clobber:
	@(cd doxygen; perl buildall.pl clobber)
	@make --directory openssl --no-print-directory clobber
	@make --directory libcurl --no-print-directory clobber
	@make --directory Xerces --no-print-directory clobber
	@make --directory zlib --no-print-directory clobber

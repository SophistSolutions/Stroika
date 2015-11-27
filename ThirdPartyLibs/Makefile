ifneq ($(CONFIGURATION),)
-include ../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk
endif


.PHONY:	all clean clobber check
.NOTPARALLEL:



all:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i;\
	done
else
	@../ScriptsLib/PrintLevelLeader.sh $(MAKELEVEL) && echo "Building Stroika {$(CONFIGURATION)} ThirdPartyLibs:"
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
endif


check:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory check CONFIGURATION=$$i;\
	done
else
	@../ScriptsLib/PrintLevelLeader.sh $(MAKELEVEL) && echo "Checking Stroika {$(CONFIGURATION)} ThirdPartyLibs:"
ifeq ($(qBuildThirdPartyProducts_ZLib), 1)
	@$(MAKE) --directory zlib --no-print-directory check CONFIGURATION=$(CONFIGURATION)
endif
ifeq ($(qBuildThirdPartyProducts_OpenSSL), 1)
	@$(MAKE) --directory openssl --no-print-directory check CONFIGURATION=$(CONFIGURATION)
endif
ifeq ($(qBuildThirdPartyProducts_libcurl), 1)
	@$(MAKE) --directory curl --no-print-directory check CONFIGURATION=$(CONFIGURATION)
endif
ifeq ($(qBuildThirdPartyProducts_LZMA), 1)
	@$(MAKE) --directory lzma --no-print-directory check CONFIGURATION=$(CONFIGURATION)
endif
ifeq ($(qBuildThirdPartyProducts_Xerces), 1)
	@$(MAKE) --directory Xerces --no-print-directory check CONFIGURATION=$(CONFIGURATION)
endif
endif


clean:
	@#ok to call with NO CONFIG (does all)
	@$(MAKE) --directory zlib --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory openssl --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory curl --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory lzma --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory Xerces --no-print-directory clean CONFIGURATION=$(CONFIGURATION)


clobber:
ifeq ($(CONFIGURATION),)
	@echo -n "Stroika ThirdPartyLibs clobber..."
else
	@echo -n "Stroika {$(CONFIGURATION)} ThirdPartyLibs clobber..."
endif
	@#ok to call with NO CONFIG (does all)
	@$(MAKE) --directory zlib --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory openssl --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory curl --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory lzma --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory Xerces --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@echo "   ...done"

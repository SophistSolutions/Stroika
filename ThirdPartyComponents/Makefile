ifneq ($(CONFIGURATION),)
-include ../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk
endif


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
	@../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Building Stroika {$(CONFIGURATION)} ThirdPartyComponents:"
ifeq ($(qBuildThirdPartyComponents_ZLib), 1)
	@$(MAKE) --directory zlib --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_OpenSSL), 1)
	@$(MAKE) --directory openssl --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_libcurl), 1)
	@$(MAKE) --directory curl --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_LZMA), 1)
	@$(MAKE) --directory lzma --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(qBuildThirdPartyComponents_Xerces), 1)
	@$(MAKE) --directory Xerces --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
endif


check:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory check CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	done
else
	@../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Checking Stroika {$(CONFIGURATION)} ThirdPartyComponents:"
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
endif


clean:
ifeq ($(CONFIGURATION),)
	@../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) -n "Stroika ThirdPartyComponents clean..."
else
	@../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) -n "Stroika {$(CONFIGURATION)} ThirdPartyComponents clean..."
endif
	@#ok to call with NO CONFIG (does all)
	@$(MAKE) --directory zlib --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory openssl --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory curl --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory lzma --no-print-directory clean CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory Xerces --no-print-directory clean CONFIGURATION=$(CONFIGURATION)


clobber:
ifeq ($(CONFIGURATION),)
	@../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) -n "Stroika ThirdPartyComponents clobber..."
else
	@../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) -n "Stroika {$(CONFIGURATION)} ThirdPartyComponents clobber..."
endif
	@#ok to call with NO CONFIG (does all)
	@$(MAKE) --directory zlib --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory openssl --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory curl --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory lzma --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(MAKE) --directory Xerces --no-print-directory clobber CONFIGURATION=$(CONFIGURATION)
	@$(ECHO) "done"

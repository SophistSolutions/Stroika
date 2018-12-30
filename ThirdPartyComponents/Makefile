export StroikaRoot?=$(shell realpath ../)/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif


.PHONY:	all clean clobber check




#Handy shortcut
CONFIGURATION_TAGS?=$(TAGS)


APPLY_CONFIGS=$(or $(CONFIGURATION), $(shell ScriptsLib/GetConfigurations --config-tags "$(CONFIGURATION_TAGS)"))



ifneq ($(CONFIGURATION),)

ifeq ($(qFeatureFlag_ActivePerl), use)
	BUILD_ActivePerl=1
endif
ifeq ($(qFeatureFlag_boost), use)
	BUILD_boost=1
endif
ifeq ($(qFeatureFlag_LibCurl), use)
	BUILD_curl=1
endif
ifeq ($(qFeatureFlag_LZMA), use)
	BUILD_lzma=1
endif
ifeq ($(qFeatureFlag_OpenSSL), use)
	BUILD_openssl=1
endif
ifeq ($(qFeatureFlag_sqlite), use)
	BUILD_sqlite=1
endif
ifeq ($(qFeatureFlag_Xerces), use)
	BUILD_Xerces=1
endif
ifeq ($(qFeatureFlag_WIX), use)
	BUILD_WIX=1
endif
ifeq ($(qFeatureFlag_ZLib), use)
	BUILD_zlib=1
endif



SubDirs=
ifeq ($(BUILD_ActivePerl), 1)
	SubDirs+= ActivePerl
endif
ifeq ($(BUILD_boost), 1)
	SubDirs+= boost
endif
ifeq ($(BUILD_curl), 1)
	SubDirs+= curl
endif
ifeq ($(BUILD_lzma), 1)
	SubDirs+= lzma
endif
ifeq ($(BUILD_openssl), 1)
	SubDirs+= openssl
endif
ifeq ($(BUILD_sqlite), 1)
	SubDirs+= sqlite
endif
ifeq ($(BUILD_Xerces), 1)
	SubDirs+= Xerces
endif
ifeq ($(BUILD_WIX), 1)
	SubDirs+= WIX
endif
ifeq ($(BUILD_zlib), 1)
	SubDirs+= zlib
endif


##WINDOWS ONLY
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq ($(BUILD_openssl), 1)
openssl:	ActivePerl
endif
endif


ifeq ($(BUILD_boost), 1)
ifeq ($(BUILD_zlib), 1)
boost:		zlib
endif
endif

ifeq ($(BUILD_curl), 1)
ifeq ($(BUILD_openssl), 1)
curl:		openssl
endif
endif

ifeq ($(BUILD_openssl), 1)
ifeq ($(BUILD_zlib), 1)
openssl:	zlib
endif
endif

###NOTE - because we set network:BOOL:OFF - Xerces doesn't depend on CURL (but we could make it so if we wanted, but then update dependencies
### in ThirdPartyLibs/Makefile
#ifeq ($(BUILD_Xerces), 1)
#ifeq ($(BUILD_curl), 1)
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
ifeq ($(BUILD_boost), 1)
	@$(MAKE) --directory boost --silent check-prerequisite-tools MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(BUILD_openssl), 1)
	@$(MAKE) --directory openssl --silent check-prerequisite-tools MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(BUILD_curl), 1)
	@$(MAKE) --directory curl --silent check-prerequisite-tools MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(BUILD_lzma), 1)
	@$(MAKE) --directory lzma --silent check-prerequisite-tools MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(BUILD_sqlite), 1)
	@$(MAKE) --directory sqlite --silent check-prerequisite-tools MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(BUILD_Xerces), 1)
	@$(MAKE) --directory Xerces --silent check-prerequisite-tools MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(BUILD_WIX), 1)
	@$(MAKE) --directory WIX --silent check-prerequisite-tools MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(BUILD_zlib), 1)
	@$(MAKE) --directory zlib --silent check-prerequisite-tools MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
endif




check:
ifeq ($(CONFIGURATION),)
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	done
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika ThirdPartyComponents Check {$(CONFIGURATION)}:"
ifeq ($(BUILD_boost), 1)
	@$(MAKE) --directory boost --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(BUILD_curl), 1)
	@$(MAKE) --directory curl --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(BUILD_lzma), 1)
	@$(MAKE) --directory lzma --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(BUILD_openssl), 1)
	@$(MAKE) --directory openssl --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(BUILD_sqlite), 1)
	@$(MAKE) --directory sqlite --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(BUILD_Xerces), 1)
	@$(MAKE) --directory Xerces --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(BUILD_WIX), 1)
	@$(MAKE) --directory WIX --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif
ifeq ($(BUILD_zlib), 1)
	@$(MAKE) --directory zlib --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
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
	@$(MAKE) --directory curl --silent $@
	@$(MAKE) --directory lzma --silent $@
	@$(MAKE) --directory openssl --silent $@
	@$(MAKE) --directory sqlite --silent $@
	@$(MAKE) --directory Xerces --silent $@
	@$(MAKE) --directory WIX --silent $@
	@$(MAKE) --directory zlib --silent $@

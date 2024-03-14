StroikaRoot:=$(abspath ../)/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif


#Handy shortcut
CONFIGURATION_TAGS?=$(TAGS)


APPLY_CONFIGS=$(or $(CONFIGURATION), $(shell ../ScriptsLib/GetConfigurations --config-tags "$(CONFIGURATION_TAGS)"))



ifneq ($(CONFIGURATION),)

ifeq ($(qFeatureFlag_boost), use)
	BUILD_boost=1
endif
ifeq ($(qFeatureFlag_LibCurl), use)
	BUILD_curl=1
endif
ifeq ($(qFeatureFlag_LZMA), use)
	BUILD_lzma=1
endif
ifeq ($(qFeatureFlag_fmtlib), use)
	BUILD_fmtlib=1
endif
ifeq ($(qFeatureFlag_GoogleTest), use)
	BUILD_GoogleTest=1
endif
ifeq ($(qFeatureFlag_OpenSSL), use)
	BUILD_openssl=1
endif
ifeq ($(qFeatureFlag_sqlite), use)
	BUILD_sqlite=1
endif
ifeq ($(qFeatureFlag_StrawberryPerl), use)
	BUILD_StrawberryPerl=1
endif
ifeq ($(qFeatureFlag_WIX), use)
	BUILD_WIX=1
endif
ifeq ($(qFeatureFlag_Xerces), use)
	BUILD_Xerces=1
endif
ifeq ($(qFeatureFlag_libxml2), use)
	BUILD_libxml2=1
endif
ifeq ($(qFeatureFlag_ZLib), use)
	BUILD_zlib=1
endif



SubDirs=
ifeq ($(BUILD_boost), 1)
	SubDirs+= boost
endif
ifeq ($(BUILD_curl), 1)
	SubDirs+= curl
endif
ifeq ($(BUILD_fmtlib), 1)
	SubDirs+= fmtlib
endif
ifeq ($(BUILD_GoogleTest), 1)
	SubDirs+= GoogleTest
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
ifeq ($(BUILD_StrawberryPerl), 1)
	SubDirs+= StrawberryPerl
endif
ifeq ($(BUILD_WIX), 1)
	SubDirs+= WIX
endif
ifeq ($(BUILD_Xerces), 1)
	SubDirs+= Xerces
endif
ifeq ($(BUILD_libxml2), 1)
	SubDirs+= libxml2
endif
ifeq ($(BUILD_zlib), 1)
	SubDirs+= zlib
endif


##WINDOWS ONLY
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq ($(BUILD_openssl), 1)
openssl:	StrawberryPerl
endif
endif

ifeq ($(BUILD_libxml2), 1)
ifeq ($(BUILD_zlib), 1)
libxml2:		zlib
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
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika ThirdPartyComponents Build {$(CONFIGURATION)}:"
ifeq ($(SubDirs),)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "none"
else
	@$(MAKE) --silent $(SubDirs)
endif
endif


check check-prerequisite-tools:
ifeq ($(CONFIGURATION),)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents $(call FUNCTION_CAPITALIZE_WORD,$@): "
else
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)}: "
endif
ifeq ($(CONFIGURATION),)
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	done
else
	@for subdir in $(SubDirs) ; do \
		$(MAKE) --directory $$subdir --silent $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)); \
	done
endif


clean clobber:
ifeq ($(CONFIGURATION),)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Stroika/ThirdPartyComponents $(call FUNCTION_CAPITALIZE_WORD,$@) ... "
else
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Stroika/ThirdPartyComponents $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)} ... "
endif
	@for subdir in $(SubDirs) ; do \
		$(MAKE) --directory $$subdir --silent $@; \
	done
	@$(ECHO) done


distclean:
	@cd Origs-Cache ; find . ! -name 'doxygen-1.8.2-setup.exe' ! -name 'doxygen-1.8.2-Windows-Installed.tar.gz' ! -name '.gitignore' -type f -exec rm -f {} +

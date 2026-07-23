StroikaRoot	:=	$(abspath ../)/

include $(StroikaRoot)Build/ScriptsLib/SharedMakeVariables-Default.mk

#Handy shortcut
CONFIGURATION_TAGS?=$(TAGS)


APPLY_CONFIGS=$(or $(CONFIGURATION), $(shell ../Build/ScriptsLib/GetConfigurations --config-tags "$(CONFIGURATION_TAGS)"))


# For clean/clobber - when we might not have a configuration
AllSubDirs_= \
	boost \
	curl \
	fmtlib \
	GoogleTest \
	lzma \
	mongo-cxx-driver \
	openssl \
	sqlite \
	StrawberryPerl \
	WIX \
	Xerces \
	libxml2 \
	zlib \
	zstd



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
ifeq ($(qFeatureFlag_mongocxxdriver), use)
	BUILD_mongocxxdriver=1
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
ifeq ($(qFeatureFlag_zstd), use)
	BUILD_zstd=1
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
ifeq ($(BUILD_mongocxxdriver), 1)
	SubDirs+= mongo-cxx-driver
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
ifeq ($(BUILD_zstd), 1)
	SubDirs+= zstd
endif


##WINDOWS ONLY
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
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
ifeq ($(BUILD_zstd), 1)
boost:		zstd
endif
endif

ifeq ($(BUILD_curl), 1)
ifeq ($(BUILD_openssl), 1)
curl:		openssl
endif
ifeq ($(BUILD_zstd), 1)
curl:		zstd
endif
endif

ifeq ($(BUILD_mongocxxdriver), 1)
ifeq ($(BUILD_openssl), 1)
mongo-cxx-driver:		openssl
endif
ifeq ($(BUILD_zlib), 1)
mongo-cxx-driver:		zlib
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
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika ThirdPartyComponents Build {$(CONFIGURATION)}:"
ifeq ($(SubDirs),)
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "none"
else
	@$(MAKE) --silent $(SubDirs)
endif
endif


check check-prerequisite-tools:
ifeq ($(CONFIGURATION),)
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents $(call FUNCTION_CAPITALIZE_WORD,$@): "
else
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)}: "
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
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Stroika/ThirdPartyComponents $(call FUNCTION_CAPITALIZE_WORD,$@) ... "
else
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Stroika/ThirdPartyComponents $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)} ... "
endif
	@for subdir in $(AllSubDirs_) ; do \
		$(MAKE) --directory $$subdir --silent $@; \
	done
	@$(ECHO) done


distclean:
	@for subdir in $(AllSubDirs_) ; do \
		$(MAKE) --directory $$subdir --silent clobber; \
	done
	@cd Origs-Cache ; find . ! -name 'doxygen-1.8.2-setup.exe' ! -name 'doxygen-1.8.2-Windows-Installed.tar.gz' ! -name '.gitignore' -type f -exec rm -f {} +

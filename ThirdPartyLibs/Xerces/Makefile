CONFIGURATION 			?=	$(shell perl ../../ScriptsLib/GetDefaultConfiguration.pl)
ProjectPlatformSubdir	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)

.DEFAULT_GOAL := all
.PHONY:	all clean clobber check

MIRROR_PREFIX_=http://psg.mtu.edu/pub/apache/
VERSION=3.1.2
BASENAME=xerces-c-$(VERSION)
ZIPPEDFILE=../Origs-Cache/$(BASENAME).tar.gz
TARFILE=$(BASENAME).tar
EXTRACTED_DIRNAME=$(BASENAME)
SLINKDIRNAME=$(BASENAME)
USE_DIRNAME=CURRENT

TRGDIR=CURRENT

FETCHURL=$(MIRROR_PREFIX_)/xerces/c/3/sources/$(BASENAME).tar.gz


BUILDS_ROOT=../../Builds/

ifeq (Linux,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Libs/libxerces-c.a				\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes/xercesc/dom/DOM.hpp
else
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Libs/xerces-c_static_3D.lib					\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes/xercesc/dom/DOM.hpp				\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Libs/xerces-c_static_3.lib				\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes/xercesc/dom/DOM.hpp				\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Libs/xerces-c_static_3.lib	\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes/xercesc/dom/DOM.hpp	\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Libs/xerces-c_static_3.lib		\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes/xercesc/dom/DOM.hpp		\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Libs/xerces-c_static_3D.lib					\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes/xercesc/dom/DOM.hpp				\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Libs/xerces-c_static_3.lib				\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes/xercesc/dom/DOM.hpp				\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Libs/xerces-c_static_3.lib		\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes/xercesc/dom/DOM.hpp
endif



all:		$(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory check


$(PRODUCED_OUTPUT_ARTIFACTS):	CURRENT/Build
	
CURRENT/Build:
	@echo "   Building Stroika/ThirdPartyLibs/xerces $(VERSION)..."
	@$(MAKE) --no-print-directory doCreate
	@$(MAKE) --no-print-directory doBuilds


clean:
	@rm -rf $(EXTRACTED_DIRNAME) CURRENT


check:
	@echo -n "      ...Checking..."
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			echo "   FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@echo "   Stroika/ThirdPartyLibs/Xerces - [Succeeded]";


clobber:
	@rm -rf $(EXTRACTED_DIRNAME) CURRENT $(BUILDS_ROOT)*/ThirdPartyLibs/*xerces*


$(ZIPPEDFILE):
	@echo -n "      ..."
	wget --quiet --tries=10 -O $(ZIPPEDFILE) $(FETCHURL) || (rm -f $(ZIPPEDFILE) && false)


doCreate:	$(ZIPPEDFILE)
	@if [ -e  CURRENT/src/xercesc/dom/impl/DOMLocatorImpl.hpp ]; then\
		echo "      ...CURRENT dir already exists";\
	else\
		echo "      ...Creating $(TRGDIR)...";\
		rm -rf $(USE_DIRNAME) CURRENT;\
		tar xf $(ZIPPEDFILE) --no-same-owner;\
		mv $(EXTRACTED_DIRNAME) $(TRGDIR);\
		$(MAKE) --no-print-directory doPatching_;\
	fi


doPatching_:
	@echo "      ...Patching Xerces...";
	@(cd CURRENT; tar xf ../Patches/VC11Projects.tar.gz --no-same-owner);
	@(cd CURRENT; tar xf ../Patches/VC12Projects.tar.gz --no-same-owner);
	@(cd CURRENT; tar xf ../Patches/VC14Projects.tar.gz --no-same-owner);
ifeq (AIX,$(shell uname))
	@patch --quiet -p0 CURRENT/configure < Patches/AIX-autoconf-fail.PATCH
endif



doBuilds:
	@echo "      ...Compiling REDIRECT to BUILD.OUT..."
	@./buildall_.pl build > BUILD.OUT 2>&1
	@echo "      ...Copying builds to ... $(BUILDS_ROOT)"
ifeq (Linux,$(ProjectPlatformSubdir))
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs && cp CURRENT/src/.libs/libxerces-c.a $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Libs
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes/
else
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Libs && cp "CURRENT/Build/Win32/VC12/Static Debug"/xerces-c_static_3* $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Libs
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Libs && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Libs
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Libs && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Libs
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Libs && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Libs
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Libs && cp "CURRENT/Build/Win64/VC12/Static Debug"/xerces-c_static_3* $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Libs
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Libs && cp "CURRENT/Build/Win64/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Libs
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Libs && cp "CURRENT/Build/Win64/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Libs
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes/
endif

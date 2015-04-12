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

UNAME := $(shell uname)

BUILDS_ROOT=../../Builds/



all:
	@echo "Building Stroika/ThirdPartyLibs/xerces $(VERSION)..."
	@$(MAKE) --no-print-directory doCreate
	@$(MAKE) --no-print-directory doBuild
	@$(MAKE) --no-print-directory check

clean:
	@rm -rf $(EXTRACTED_DIRNAME) CURRENT

check:
	@echo -n "   ...Checking..."
ifeq (,$(findstring CYGWIN,$(UNAME)))
	@if [ ! -e $(BUILDS_ROOT)/DefaultConfiguration/ThirdPartyLibs/libxerces-c.a ]; then\
		echo "   FAILED: missing $(BUILDS_ROOT)/DefaultConfiguration/ThirdPartyLibs/libxerces-c.a;\
	else\
		echo "   Stroika/ThirdPartyLibs/Xerces - [Succeeded]";\
	fi
else
	@if [ ! -e $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/xerces-c_static_3D.lib ]; then\
		echo "   FAILED: missing $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/xerces-c_static_3D.lib";\
	elif [ ! -e $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/xerces-c_static_3.lib ]; then\
		echo "   FAILED: missing (BUILDS_ROOT)Release-U-32/ThirdPartyLibs/xerces-c_static_3.lib";\
	elif [ ! -e $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/xerces-c_static_3.lib ]; then\
		echo "   FAILED: missing (BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/xerces-c_static_3.lib";\
	elif [ ! -e $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/xerces-c_static_3.lib ]; then\
		echo "   FAILED: missing (BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/xerces-c_static_3.lib";\
	elif [ ! -e $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/xerces-c_static_3D.lib ]; then\
		echo "   FAILED: missing (BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/xerces-c_static_3D.lib";\
	elif [ ! -e $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/xerces-c_static_3.lib ]; then\
		echo "   FAILED: missing (BUILDS_ROOT)Release-U-64/ThirdPartyLibs/xerces-c_static_3.lib";\
	elif [ ! -e $(BUILDS_ROOT)Release-Logging-U-64//ThirdPartyLibs/xerces-c_static_3.lib ]; then\
		echo "   FAILED: missing (BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/xerces-c_static_3.lib";\
	else\
		echo "   Stroika/ThirdPartyLibs/Xerces - [Succeeded]";\
	fi
endif


clobber:
	@rm -rf $(EXTRACTED_DIRNAME) CURRENT $(BUILDS_ROOT)*/ThirdPartyLibs/*xerces*

$(ZIPPEDFILE):
	@echo -n "   ..."
	wget --quiet --tries=10 --no-check-certificate -O $(ZIPPEDFILE) $(FETCHURL) || (rm -f $(ZIPPEDFILE) && false)


doCreate:	$(ZIPPEDFILE)
	@if [ -e  CURRENT/src/xercesc/dom/impl/DOMLocatorImpl.hpp ]; then\
		echo "   ...CURRENT dir already exists";\
	else\
		echo "   ...Creating $(TRGDIR)...";\
		rm -rf $(USE_DIRNAME) CURRENT;\
		tar xf $(ZIPPEDFILE) --no-same-owner;\
		mv $(EXTRACTED_DIRNAME) $(TRGDIR);\
		#ln -s CURRENT $(SLINKDIRNAME);\
		echo "   ...Patching Xerces...";\
		(cd CURRENT; tar xf ../Patches/VC11Projects.tar.gz --no-same-owner);\
		(cd CURRENT; tar xf ../Patches/VC12Projects.tar.gz --no-same-owner);\
		chmod --recursive +rwx CURRENT/projects;\
	fi

doBuild:
	@echo "   ...Compiling REDIRECT to BUILD.OUT..."
	@./buildall_.pl build > BUILD.OUT 2>&1
	@echo "   ...Copying builds to ... $(BUILDS_ROOT)"
ifeq (,$(findstring CYGWIN,$(shell uname)))
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs && cp CURRENT/src/.libs/libxerces-c.a $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/
else
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs && cp "CURRENT/Build/Win32/VC12/Static Debug"/xerces-c_static_3* $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs && cp "CURRENT/Build/Win64/VC12/Static Debug"/xerces-c_static_3* $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs && cp "CURRENT/Build/Win64/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs && cp "CURRENT/Build/Win64/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/
endif

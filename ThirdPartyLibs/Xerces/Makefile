ifneq ($(CONFIGURATION),)
	ProjectPlatformSubdir	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
endif


.DEFAULT_GOAL := all
.PHONY:	all clean clobber check doBuild doCreate doPatch

.NOTPARALLEL:


VERSION=3.1.2
BASENAME=xerces-c-$(VERSION)
ZIPPEDFILE=../Origs-Cache/$(BASENAME).tar.gz
TARFILE=$(BASENAME).tar
EXTRACTED_DIRNAME=$(BASENAME)

MAKE_INDENT_LEVEL?=$(MAKELEVEL)


FETCHURLS=
FETCHURLS+=http://mirror.sdunix.com/apache//xerces/c/3/sources/$(BASENAME).tar.gz
FETCHURLS+=http://psg.mtu.edu/pub/apache//xerces/c/3/sources/$(BASENAME).tar.gz


BUILDS_ROOT=../../Builds/

ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyLibs/lib/libxerces-c.a				\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyLibs/include/xercesc/dom/DOM.hpp
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/lib/xerces-c_static_3D.lib					\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/include/xercesc/dom/DOM.hpp				\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/lib/xerces-c_static_3.lib				\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/include/xercesc/dom/DOM.hpp				\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/lib/xerces-c_static_3.lib	\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/include/xercesc/dom/DOM.hpp	\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/lib/xerces-c_static_3.lib		\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/include/xercesc/dom/DOM.hpp		\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/lib/xerces-c_static_3D.lib					\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/include/xercesc/dom/DOM.hpp				\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/lib/xerces-c_static_3.lib				\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/include/xercesc/dom/DOM.hpp				\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/lib/xerces-c_static_3.lib		\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/include/xercesc/dom/DOM.hpp
endif



all:
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Building Stroika/ThirdPartyLibs/xerces $(VERSION):"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	@$(MAKE) --no-print-directory --silent CONFIGURATION=$(CONFIGURATION) $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) check


$(PRODUCED_OUTPUT_ARTIFACTS):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) doCreate
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) doBuilds


clean:
	@rm -rf $(EXTRACTED_DIRNAME) CURRENT


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(BUILDS_ROOT)*/ThirdPartyLibs/lib/*xerces* $(BUILDS_ROOT)*/ThirdPartyLibs/include/*xerces*
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif


check:
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo -n "...Checking..."
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			echo "   FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@echo "Stroika/ThirdPartyLibs/Xerces -  [Succeeded]";


$(ZIPPEDFILE):
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo -n "..."
	@../../ScriptsLib/WebGet.sh $(ZIPPEDFILE) $(FETCHURLS)


doCreate:	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "...Creating CURRENT...";
	@rm -rf CURRENT;
	@tar xf $(ZIPPEDFILE) --no-same-owner;
	@mv $(EXTRACTED_DIRNAME) CURRENT;
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) doPatching_


doPatching_:
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "...Patching Xerces...";
	@(cd CURRENT; tar xf ../Patches/VC12Projects.tar.gz --no-same-owner);
	@(cd CURRENT; tar xf ../Patches/VC14Projects.tar.gz --no-same-owner);
ifeq (AIX,$(shell uname))
	@patch --quiet -p0 CURRENT/configure < Patches/AIX-autoconf-fail.PATCH
endif


doBuilds:
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "...Compiling REDIRECT to BUILD.OUT..."
ifeq (Unix,$(ProjectPlatformSubdir))
	@cd CURRENT ; ./configure --enable-static --disable-shared --without-icu --without-curl 2>&1 > BUILD.OUT
	@$(MAKE) --directory CURRENT --no-print-directory -s CONFIGURATION=$(CONFIGURATION) all 2>&1 >> BUILD.OUT
else
	@CONFIGURATION=$(CONFIGURATION) ./buildall_.pl build > BUILD.OUT 2>&1
endif
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "...Copying builds to ... $(BUILDS_ROOT)"
ifeq (Unix,$(ProjectPlatformSubdir))
	@mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyLibs && cp CURRENT/src/.libs/libxerces-c.a $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyLibs/lib
	@mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyLibs/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyLibs/include/
else
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/lib && cp "CURRENT/Build/Win32/VC12/Static Debug"/xerces-c_static_3* $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/lib
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/lib && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/lib
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/lib && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/lib
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/lib && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/lib
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/lib && cp "CURRENT/Build/Win64/VC12/Static Debug"/xerces-c_static_3* $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/lib
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/lib && cp "CURRENT/Build/Win64/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/lib
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/lib && cp "CURRENT/Build/Win64/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/lib
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/include/
endif

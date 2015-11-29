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

PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/xercesc/dom/DOM.hpp

ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/libxerces-c.a
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq (Debug,$(findstring Debug, $(CONFIGURATION)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/xerces-c_static_3D.lib
endif
ifeq (Release,$(findstring Release, $(CONFIGURATION)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/xerces-c_static_3.lib
endif
endif



all:
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Building Stroika/ThirdPartyComponents/xerces $(VERSION):"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	@$(MAKE) --no-print-directory --silent CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))  check


$(PRODUCED_OUTPUT_ARTIFACTS):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doCreate
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doBuilds


clean:
	@rm -rf $(EXTRACTED_DIRNAME) CURRENT


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(BUILDS_ROOT)*/ThirdPartyComponents/lib/*xerces* $(BUILDS_ROOT)*/ThirdPartyComponents/include/*xerces*
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif


check:
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo -n "Checking..."
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
	@echo " Stroika/ThirdPartyComponents/Xerces -  [Succeeded]";


$(ZIPPEDFILE):
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo -n "..."
	@../../ScriptsLib/WebGet.sh $(ZIPPEDFILE) $(FETCHURLS)


doCreate:	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo -n "Creating CURRENT...";
	@rm -rf CURRENT;
	@tar xf $(ZIPPEDFILE) --no-same-owner;
	@mv $(EXTRACTED_DIRNAME) CURRENT;
	@echo "done"
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doPatching_


doPatching_:
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo -n "Patching Xerces...";
	@(cd CURRENT; tar xf ../Patches/VC12Projects.tar.gz --no-same-owner);
	@(cd CURRENT; tar xf ../Patches/VC14Projects.tar.gz --no-same-owner);
ifeq (AIX,$(shell uname))
	@patch --quiet -p0 CURRENT/configure < Patches/AIX-autoconf-fail.PATCH
endif
	@echo "done"


doBuilds:
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo -n "Compiling (REDIRECT to CURRENT/BUILD.OUT)..."
ifeq (Unix,$(ProjectPlatformSubdir))
	@cd CURRENT ; ./configure --enable-static --disable-shared --without-icu --without-curl 2>&1 > BUILD.OUT
	@$(MAKE) --directory CURRENT --no-print-directory -s CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  all 2>&1 >> CURRENT/BUILD.OUT
else
	@CONFIGURATION=$(CONFIGURATION) ./buildall_windows_.pl build > CURRENT/BUILD.OUT 2>&1
endif
	@echo "done"
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo -n "Copying builds to $(BUILDS_ROOT) ..."
ifeq (Unix,$(ProjectPlatformSubdir))
	@mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents && cp CURRENT/src/.libs/libxerces-c.a $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib
	@mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/
else
ifeq ($(CONFIGURATION),Debug-U-32)
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib && cp "CURRENT/Build/Win32/VC12/Static Debug"/xerces-c_static_3* $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-DbgMemLeaks-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-Logging-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Debug-U-64)
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib && cp "CURRENT/Build/Win64/VC12/Static Debug"/xerces-c_static_3* $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-U-64)
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib && cp "CURRENT/Build/Win64/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-Logging-U-64)
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib && cp "CURRENT/Build/Win64/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/include/
endif
endif
	@echo "done"

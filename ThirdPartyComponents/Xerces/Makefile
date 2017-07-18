ifneq ($(CONFIGURATION),)
	ProjectPlatformSubdir	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
endif


.DEFAULT_GOAL := all
.PHONY:	all clean clobber check doBuilds_ doCreate_ doPatching_

SHELL=/bin/bash

.NOTPARALLEL:


VERSION=3.1.4
BASENAME=xerces-c-$(VERSION)
ZIPPEDFILE=../Origs-Cache/$(BASENAME).tar.gz
TARFILE=$(BASENAME).tar
EXTRACTED_DIRNAME=$(BASENAME)

MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ../../ScriptsLib/GetDefaultShellVariable.sh ECHO)

#
# CHECK FOR UPDATED VERSIONS http://xerces.apache.org/xerces-c/
#

FETCHURLS=
#
#Most Mirrors only have the current release so when xercesc updated, these start
#failing (bad when building old stroika)
#
FETCHURLS+=http://apache.cs.utah.edu//xerces/c/3/sources/$(BASENAME).tar.gz
FETCHURLS+=http://www.apache.org/dist/xerces/c/3/sources/$(BASENAME).tar.gz
FETCHURLS+=http://www-us.apache.org/dist/xerces/c/3/sources/$(BASENAME).tar.gz

#
#ARCHIVE sites have old versions which matters when you are building an
#older verison of Stroika
#
FETCHURLS+=http://archive.apache.org/dist/xerces/c/3/sources/$(BASENAME).tar.gz




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


ifeq (Unix,$(ProjectPlatformSubdir))
CC=$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) CompilerDriver-C)
CXX=$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) CompilerDriver-C++)
#CFLAGS=$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) EXTRA_COMPILER_ARGS)

#NOT a good solution, but setting CFLAGS arg to configure causes grave disorder
#CC+=$(CFLAGS)
#CXX+=$(CFLAGS)
endif


all:
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Building Stroika/ThirdPartyComponents/xerces $(VERSION):"
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
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doCreate_
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doBuilds_


clean:
	@rm -rf $(EXTRACTED_DIRNAME) CURRENT


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(BUILDS_ROOT)*/ThirdPartyComponents/lib/*xerces* $(BUILDS_ROOT)*/ThirdPartyComponents/include/*xerces*
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif


check-prerequisite-tools:
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) -n "Checking Prerequisite tools for Xerces..."
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@#https://stroika.atlassian.net/browse/STK-437
	@#NOT QUITE FIXED - BUT a start
	@#very hard to track down subtle bug - avoidance - @see http://stackoverflow.com/questions/34074925/vs-2015-cannot-open-compiler-generated-file-invalid-argument
	@#This doesnt appear broken on windows 10 - but does on windows 8.1 - must investigate further
	@EXTRACHAR=37 && MAXCHAR=49 && SUM=$(($$EXTRACHAR + $$MAXCHAR)) && PWD=`pwd` && ADJ_PWDLEN=$$(($${#PWD} - $$EXTRACHAR)) && if [[ $$ADJ_PWDLEN -gt $$MAXCHAR ]] ; then  $(ECHO) && ../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) "WARNING: STRLEN(Stroika-Root-Directory) == $$ADJ_PWDLEN, and can be no more than $$MAXCHAR characters on windows 8.1 to avoid Xerces build problem" && ../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL); fi;
endif
	@$(ECHO) "done";

check:
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) -n "Checking..."
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			$(ECHO) "   FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@$(ECHO) " Stroika/ThirdPartyComponents/Xerces -  [Succeeded]";


$(ZIPPEDFILE):
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1))
	@../../ScriptsLib/WebGet.sh $(ZIPPEDFILE) $(FETCHURLS)


doCreate_:	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) -n "Creating CURRENT...";
	@mkdir -p CURRENT && rm -rf CURRENT/*
ifeq ($(shell uname -s),Darwin)
	@tar xf $(ZIPEDFILE) -C CURRENT --strip-components 1
else
	@tar xf $(ZIPEDFILE) --no-same-owner --directory CURRENT --strip-components=1
endif
	@$(ECHO) "done"
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doPatching_


doPatching_:
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) -n "Patching Xerces...";
	@(cd CURRENT; tar xf ../Patches/VC12Projects.tar.gz --no-same-owner);
	@(cd CURRENT; tar xf ../Patches/VC14Projects.tar.gz --no-same-owner);
	@(cd CURRENT; tar xf ../Patches/VC15Projects.tar.gz --no-same-owner);
	@$(ECHO) "done"

ifeq (Unix,$(ProjectPlatformSubdir))
CONFIGURE_ARGS=
CONFIGURE_ARGS+=--enable-static --disable-shared --without-icu --without-curl
#on RedHat6, I had trouble with icu, so lose the dependency - not sure why helpful; I think issue
#also exists with gnuiconv - and has todo with fact that service script (init.d stuff) sets LANG=""
CONFIGURE_ARGS+=--enable-transcoder-iconv
CONFIGURE_ARGS+=--disable-transcoder-gnuiconv
CONFIGURE_ARGS+=--disable-transcoder-icu
ifeq ($(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) CrossCompiling),true)
CONFIGURE_ARGS+= --host $(shell $(CXX) -dumpmachine)
endif
endif

doBuilds_:
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) -n "Compiling (REDIRECT to CURRENT/BUILD.OUT)..."
ifeq (Unix,$(ProjectPlatformSubdir))
	@echo "export CC=\"$(CC)\"; export CXX=\"$(CXX)\"; ./configure $(CONFIGURE_ARGS)" > CURRENT/BUILD.OUT
	@cd CURRENT; export CC="$(CC)"; export CXX="$(CXX)"; ./configure $(CONFIGURE_ARGS) 2>&1 >> BUILD.OUT
	$(MAKE) --directory CURRENT --no-print-directory -s CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) all 2>&1 >> CURRENT/BUILD.OUT
else
	@CONFIGURATION=$(CONFIGURATION) ./buildall_windows_.pl build > CURRENT/BUILD.OUT 2>&1
endif
	@$(ECHO) "done"
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) -n "Copying builds to $(BUILDS_ROOT) ..."
ifeq (Unix,$(ProjectPlatformSubdir))
	@mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib && cp CURRENT/src/.libs/libxerces-c.a $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/
else
ifeq ($(CONFIGURATION),Debug-U-32)
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib && cp "CURRENT/Build/Win32/VC12/Static Debug"/xerces-c_static_3* $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-DbgMemLeaks-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-Logging-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Debug-U-64)
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib && cp "CURRENT/Build/Win64/VC12/Static Debug"/xerces-c_static_3* $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-U-64)
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib && cp "CURRENT/Build/Win64/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-Logging-U-64)
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib && cp "CURRENT/Build/Win64/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/include/
endif
endif
	@$(ECHO) "done"

export StroikaRoot?=$(shell realpath ../..)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk


.DEFAULT_GOAL := all
.PHONY:	all clean clobber check doBuilds_



#
# CHECK FOR UPDATED VERSIONS http://www.7-zip.org/sdk.html
#



#NO - BIG CHANGES - AND NOT DOCUMENTED AND NOT CLEAR BETTER - VERSION=1700
VERSION=1604
#VERSION=1602
BASENAME=lzma$(VERSION)
ZIPPEDFILE=../Origs-Cache/$(BASENAME).7z

FETCHURLS=
FETCHURLS+=http://www.7-zip.org/a/$(BASENAME).7z


PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)/

#lose trailing / on this variable because on macos make THATDIR doesn't work with trailing slash
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/lzma)

BUILDS_ROOT=$(StroikaRoot)Builds/


ifeq (Unix,$(ProjectPlatformSubdir))
LIBS	=	-lstdc++ -lc
endif



PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)include/lzma/7z.h		\
	$(PER_CONFIGURATION_BUILDS_DIR)include/lzma/7zCrc.h		\
	$(PER_CONFIGURATION_BUILDS_DIR)include/lzma/7zTypes.h

ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/liblzma.a
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/lzma.lib
endif


OUTPUT_WORKDIR_PRETTYNAME="{StroikaRoot}/Inter.../Thir.../lzma/"



all:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/lzma Build $(VERSION) {$(CONFIGURATION)}:"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --no-print-directory --silent CURRENT $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check


$(PRODUCED_OUTPUT_ARTIFACTS):	PRODUCED_OUTPUT_ARTIFACTS_

.PHONY: PRODUCED_OUTPUT_ARTIFACTS_
PRODUCED_OUTPUT_ARTIFACTS_:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --no-print-directory check_prerequisites_
	@$(MAKE) --silent $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)
	@$(MAKE) --no-print-directory doBuilds_


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ../../ScriptsLib/WebGet.sh $(ZIPPEDFILE) $(FETCHURLS)


check-prerequisite-tools:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking Prerequisite tools for lzma ... "
	@$(MAKE) --no-print-directory --silent check_prerequisites_
	@$(ECHO) "done";


check_prerequisites_:
	@{ type 7za > /dev/null 2>&1; } || { ../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) && ../../ScriptsLib/GetMessageForMissingTool.sh 7za && exit 1; }


doBuilds_:
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "lzma ${VERSION} - Build: see `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)BUILD.OUT"` ..."
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) doBuildInternal_ > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/BUILD.OUT 2>&1


doBuildInternal_:
ifeq (Unix,$(ProjectPlatformSubdir))
	$(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) ProjectPlatformSubdir=$(ProjectPlatformSubdir) CC="$(CC)" CXX="$(CXX)" AR="$(AR)" LIBS="$(LIBS)"
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/lzma.a  $(PER_CONFIGURATION_BUILDS_DIR)lib/liblzma.a
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include/lzma && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/C/7z.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/C/7zCrc.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/C/7zTypes.h $(PER_CONFIGURATION_BUILDS_DIR)include/lzma/
	cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/lzma $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/7zDec $(PER_CONFIGURATION_BUILDS_DIR)
else
	#Build to $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) - if its 32-bit debug/relase
ifeq (U-32,$(findstring U-32,$(CONFIGURATION)))
ifeq (Debug,$(findstring Debug,$(CONFIGURATION)))
	PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)" $(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		CC="$(CC)" \
		CXX="$(CXX)" \
		CFLAGS="-nologo -W3 -MTd -Z7" \
		INCLUDE="$(WIN_INCLUDES_PATH)" \
		LIB="$(WIN_LIBS_PATH)" \
		LIBS="oleaut32.lib ole32.lib user32.lib" \
		LD="$(Linker)" \
		AR="$(AR)" \
		ARCRATEFLAGS="-nologo" \
		ARTARGETPREFIX=-out: \
		LINKTARGETPREFIX=-out: \
		LDFLAGS="-nologo -debug -incremental:no -opt:ref" \
		EXESUFFIX=.exe \
		LIBSUFFIX=lib \
		OBJSUFFIX=obj \
		all
endif
ifeq (Release,$(findstring Release,$(CONFIGURATION)))
	PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)" $(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		CC="$(CC)" \
		CXX="$(CXX)" \
		CFLAGS="-nologo -W3 -MT -O2 -Oy- -Z7" \
		INCLUDE="$(WIN_INCLUDES_PATH)" \
		LIB="$(WIN_LIBS_PATH)" \
		LIBS="oleaut32.lib ole32.lib user32.lib" \
		LD="$(Linker)" \
		AR="$(AR)" \
		ARCRATEFLAGS="-nologo" \
		ARTARGETPREFIX=-out: \
		LINKTARGETPREFIX=-out: \
		LDFLAGS="-nologo -debug -incremental:no -opt:ref" \
		EXESUFFIX=.exe \
		LIBSUFFIX=lib \
		OBJSUFFIX=obj \
		all
endif
endif

	#Build to $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) - if its 64-bit debug/relase
ifeq (U-64,$(findstring U-64,$(CONFIGURATION)))
ifeq (Debug,$(findstring Debug,$(CONFIGURATION)))
	PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)" $(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		CC="$(CC)" \
		CXX="$(CXX)" \
		CFLAGS="-nologo -W3 -MTd -Oy- -Z7" \
		INCLUDE="$(WIN_INCLUDES_PATH)" \
		LIB="$(WIN_LIBS_PATH)" \
		PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)" \
		LIBS="oleaut32.lib ole32.lib user32.lib" \
		LD="$(Linker)" \
		AR="$(AR)" \
		ARCRATEFLAGS="-nologo" \
		ARTARGETPREFIX=-out: \
		LINKTARGETPREFIX=-out: \
		LDFLAGS="-nologo -debug -incremental:no -opt:ref" \
		EXESUFFIX=.exe \
		LIBSUFFIX=lib \
		OBJSUFFIX=obj \
		all
endif
ifeq (Release,$(findstring Release,$(CONFIGURATION)))
	PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)" $(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		CC="$(CC)" \
		CXX="$(CXX)" \
		CFLAGS="-nologo -W3 -MT -O2 -Oy- -Z7" \
		INCLUDE="$(WIN_INCLUDES_PATH)" \
		LIB="$(WIN_LIBS_PATH)" \
		PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)" \
		LIBS="oleaut32.lib ole32.lib user32.lib" \
		LD="$(Linker)" \
		AR="$(AR)" \
		ARCRATEFLAGS="-nologo" \
		ARTARGETPREFIX=-out: \
		LINKTARGETPREFIX=-out: \
		LDFLAGS="-nologo -debug -incremental:no -opt:ref" \
		EXESUFFIX=.exe \
		LIBSUFFIX=lib \
		OBJSUFFIX=obj \
		all
endif
endif

	# Now copy the right output
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/lzma.lib  $(PER_CONFIGURATION_BUILDS_DIR)lib/
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include/lzma && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/C/7z.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/C/7zCrc.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/C/7zTypes.h $(PER_CONFIGURATION_BUILDS_DIR)include/lzma/
	cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/lzma.exe $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/7zDec.exe $(PER_CONFIGURATION_BUILDS_DIR)

endif


ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR):	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "lzma ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@7z x -o$@ $(ZIPPEDFILE) > /dev/null
	@if [ "$@" = "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)" ] ; then \
		cp Subdir-Makefile $@/Makefile;\
	fi


check:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking ... "
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			$(ECHO) "   FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@$(ECHO) " Stroika/ThirdPartyComponents/lzma -    [Succeeded]";


clean:
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(StroikaRoot)Builds/*/ThirdPartyComponents/lib/{lzma}* $(StroikaRoot)Builds/*/ThirdPartyComponents/include/lzma
	@rm -rf CURRENT
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif

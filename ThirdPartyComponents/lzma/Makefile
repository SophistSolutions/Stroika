###
### TODO:
###		@todo	Big cleanup needed to windows make code to be more terse (no need for some many cases now - use vars to 
###				to parameterize).
###

export StroikaRoot?=$(shell realpath ../..)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk


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

#lose trailing / on PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_ because on macos make THATDIR doesn't work with trailing slash (old WEIRD_MACOS_MAKEFILE_DIR_BUG -  MacOS XCode 10 - GNU Make 3.81)
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/lzma)


ifeq (Unix,$(ProjectPlatformSubdir))
LIBS	=	-lstdc++ -lc
endif



PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)include/lzma/7z.h		\
	$(PER_CONFIGURATION_BUILDS_DIR)include/lzma/7zCrc.h		\
	$(PER_CONFIGURATION_BUILDS_DIR)include/lzma/7zTypes.h

ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS+=		$(PER_CONFIGURATION_BUILDS_DIR)lib/liblzma.a
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
PRODUCED_OUTPUT_ARTIFACTS+=		$(PER_CONFIGURATION_BUILDS_DIR)lib/lzma.lib
endif


OUTPUT_WORKDIR_PRETTYNAME="{StroikaRoot}/Inter.../Thir.../lzma/"



all:
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/lzma Build $(VERSION) {$(CONFIGURATION)}:"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --no-print-directory --silent $(ZIPPEDFILE)
	@$(MAKE) --no-print-directory --silent CURRENT $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check


#ALL produced artifacts produced by the same thing. But if you try the same rule to make all, if parallel makes invoked on this, you get
#multiple tries to run the same rule. So remap all to making the first.
$(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(StroikaRoot)ConfigurationFiles/$(CONFIGURATION).xml
	@$(MAKE) $(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))

$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --no-print-directory check_prerequisites_
	@$(MAKE) --silent $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
	@$(MAKE) --no-print-directory doBuilds_


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ../../ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)


check-prerequisite-tools:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Checking Prerequisite tools for lzma ... "
	@$(MAKE) --no-print-directory --silent check_prerequisites_
	@$(ECHO) "done"


check_prerequisites_:
	@{ type 7za > /dev/null 2>&1; } || { ../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) && ../../ScriptsLib/GetMessageForMissingTool 7za && exit 1; }


doBuilds_:
	@../../ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "lzma ${VERSION} - Build: see `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD.OUT"` ..."
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) doBuildInternal_ > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD.OUT 2>&1


doBuildInternal_:
ifeq (Unix,$(ProjectPlatformSubdir))
	$(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) ProjectPlatformSubdir=$(ProjectPlatformSubdir) CC="$(CC)" CXX="$(CXX)" AR="$(AR)" LIBS="$(LIBS)"
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/lzma.a  $(PER_CONFIGURATION_BUILDS_DIR)lib/liblzma.a
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include/lzma && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/C/7z.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/C/7zCrc.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/C/7zTypes.h $(PER_CONFIGURATION_BUILDS_DIR)include/lzma/
	cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/lzma $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/7zDec $(PER_CONFIGURATION_BUILDS_DIR)
else
	#Build to $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) - if its 32-bit debug/relase
ifeq (U-32,$(findstring U-32,$(CONFIGURATION)))
ifeq (Debug,$(findstring Debug,$(CONFIGURATION)))
	PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)" $(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) \
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
	PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)" $(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) \
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

	#Build to $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) - if its 64-bit debug/relase
ifeq (U-64,$(findstring U-64,$(CONFIGURATION)))
ifeq (Debug,$(findstring Debug,$(CONFIGURATION)))
	PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)" $(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) \
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
	PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)" $(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) \
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
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/lzma.lib  $(PER_CONFIGURATION_BUILDS_DIR)lib/
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include/lzma && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/C/7z.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/C/7zCrc.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/C/7zTypes.h $(PER_CONFIGURATION_BUILDS_DIR)include/lzma/
	cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/lzma.exe $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/7zDec.exe $(PER_CONFIGURATION_BUILDS_DIR)

endif


ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_):	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "lzma ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@7z x -o$@ $(ZIPPEDFILE) > /dev/null
	@if [ "$@" = "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)" ] ; then \
		cp Subdir-Makefile $@/Makefile;\
	fi


check:
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Checking ... "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/lzma -    [Succeeded]";


clean:
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(StroikaRoot)Builds/*/ThirdPartyComponents/lib/{lzma}* $(StroikaRoot)Builds/*/ThirdPartyComponents/include/lzma
	@rm -rf CURRENT
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif

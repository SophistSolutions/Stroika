###
### TODO:
###		@todo	Big cleanup needed to windows make code to be more terse (no need for some many cases now - use vars to 
###				to parameterize).
###
#		@todo	NOTE - try for $(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)) and $(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))
#				(repeated in most ThirdPartyComponents makefiles)
#				doesn't work if the first produced output artifact is fine, but the others got deleted. No BIGGIE, as you can just make clobber in that
#				exceedingly rare case; but certainly a defect of this approach.

StroikaRoot=$(abspath ../..)/

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
FETCHURLS+=https://sourceforge.net/projects/sevenzip/files/LZMA%20SDK/$(BASENAME).7z


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


#OUTPUT_WORKDIR_PRETTYNAME="{StroikaRoot}Inter.../Thir.../lzma/"
OUTPUT_WORKDIR_PRETTYNAME="$(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/")"



#
# For now needed beacuse we don't pass in proper LDFLAGS so test apps dont know about extra linker flags needed for sanitizers
# -- LGP 2019-09-25
#
CFLAGS:=$(filter-out -fsanitize=%, $(CFLAGS))
CXXFLAGS:=$(filter-out -fsanitize=%, $(CXXFLAGS))


###
### got error building openssl wihtout this filter-out with clang-7 release builds - not sure it was the same message though
### and 
###		7zMain.o: file not recognized: file format not recognized
###		clang: error: linker command failed with exit code 1 (use -v to see invocation)
###		LzmaAlone.o: file not recognized: file format not recognized
###		clang: error: linker command failed with exit code 1 (use -v to see invocation)
###
###		-- LGP 2019-09-26
###
CFLAGS:=$(filter-out -flto, $(CFLAGS))
CXXFLAGS:=$(filter-out -flto, $(CXXFLAGS))


all:
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/lzma Build $(VERSION) {$(CONFIGURATION)}:"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --no-print-directory --silent $(ZIPPEDFILE)
	@$(MAKE) --no-print-directory --silent $(PRODUCED_OUTPUT_ARTIFACTS)
ifneq ($(QUICK_BUILD), 1)
	@$(MAKE) --no-print-directory --silent CURRENT
endif
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check

#ALL produced artifacts produced by the same thing. But if you try the same rule to make all, if parallel makes invoked on this, you get
#multiple tries to run the same rule. So remap all to making the first.
$(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))

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
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "lzma ${VERSION} - Checking Prerequisite tools ... "
	@$(MAKE) --no-print-directory --silent check_prerequisites_
	@$(ECHO) "done"


check_prerequisites_:
	@{ type 7za > /dev/null 2>&1; } || { ../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) && ../../ScriptsLib/GetMessageForMissingTool 7za && exit 1; }
	@#This weird redundant check for 7za is due to running WSL2 where the same stuff is in the path and there is a cygwin version in the path, but it wont work, so do this test which properly fails to trigger the error message about needing 7za installed under WSL2
	@{ 7za --help > /dev/null 2>&1; } || { ../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) && ../../ScriptsLib/GetMessageForMissingTool 7za && exit 1; }


doBuilds_:
	@../../ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "lzma ${VERSION} - Build: see ${OUTPUT_WORKDIR_PRETTYNAME}BUILD.OUT ..."
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) doBuildInternal_ > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD.OUT 2>&1



doBuildInternal_:
ifeq (Unix,$(ProjectPlatformSubdir))
	$(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) \
		AR='$(AR)' \
		CC='$(CC)' \
		CXX='$(CXX)' \
		CFLAGS='$(CFLAGS)' \
		CXXFLAGS='$(CXXFLAGS)' \
		ProjectPlatformSubdir='$(ProjectPlatformSubdir)' \
		LIBS='$(LIBS)' \
		all
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/lzma.a  $(PER_CONFIGURATION_BUILDS_DIR)lib/liblzma.a
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include/lzma && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/C/7z.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/C/7zCrc.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/C/7zTypes.h $(PER_CONFIGURATION_BUILDS_DIR)include/lzma/
	cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/lzma $(PER_CONFIGURATION_BUILDS_DIR)
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)bin && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/7zDec $(PER_CONFIGURATION_BUILDS_DIR)/bin/
else
	PATH='$(TOOLS_PATH_ADDITIONS):$(PATH)';\
		$(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) \
			CC='$(CC)' \
			CXX='$(CXX)' \
			LD='$(Linker)' \
			AR='$(LIBTOOL)' \
			ProjectPlatformSubdir='$(ProjectPlatformSubdir)' \
			INCLUDE='$(WIN_INCLUDES_PATH)' \
			LIB='$(WIN_LIBS_PATH)' \
			LIBS='oleaut32.lib ole32.lib user32.lib' \
			ARCRATEFLAGS='-nologo' \
			ARTARGETPREFIX=-out: \
			LINKTARGETPREFIX=-out: \
			OBJ_SUFFIX=${OBJ_SUFFIX} LIB_SUFFIX=${LIB_SUFFIX} EXE_SUFFIX=${EXE_SUFFIX} \
			CFLAGS='$(CFLAGS)' \
			CXXFLAGS='$(CXXFLAGS)' \
			LDFLAGS='-nologo -debug -incremental:no -opt:ref' \
			all
	# Now copy the output to the BUILD directory
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/lzma.lib  $(PER_CONFIGURATION_BUILDS_DIR)lib/
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include/lzma && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/C/7z.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/C/7zCrc.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/C/7zTypes.h $(PER_CONFIGURATION_BUILDS_DIR)include/lzma/
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)bin && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/lzma.exe $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/7zDec.exe $(PER_CONFIGURATION_BUILDS_DIR)/bin/
endif



ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_):	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "lzma ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@7za x -o$@ $(ZIPPEDFILE) > /dev/null
	@if [ "$@" = "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)" ] ; then \
		cp Subdir-Makefile $@/Makefile;\
	fi


check:
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "lzma ${VERSION} - Checking ... "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/lzma -    [Succeeded]";


clean:
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(StroikaRoot)Builds/*/ThirdPartyComponents/lib/{lzma}* $(StroikaRoot)Builds/*/ThirdPartyComponents/include/lzma
	@rm -rf CURRENT
else
	@rm -rf $(PRODUCED_OUTPUT_ARTIFACTS) $(PER_CONFIGURATION_BUILDS_DIR)include/lzma
endif

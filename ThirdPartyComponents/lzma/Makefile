###
### TODO:
###		@todo	Big cleanup needed to windows make code to be more terse (no need for some many cases now - use vars to 
###				to parameterize).
###
#		@todo	NOTE - try for $(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)) and $(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))
#				(repeated in most ThirdPartyComponents makefiles)
#				doesn't work if the first produced output artifact is fine, but the others got deleted. No BIGGIE, as you can just make clobber in that
#				exceedingly rare case; but certainly a defect of this approach.

StroikaRoot:=$(abspath ../..)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk


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


#not parallel because submakefiles use parallelism, but generally best to sequence these top level requests. Like if you say
# make clobber all you don't want those to happen at the same time. And make libraries samples wouldn't really work since all the libraries
# have to be built before the samples etc...
.NOTPARALLEL:

PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents)/
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/lzma)/

# Only needed on MacOS due to make 3.81, I believe --LGP 2024-07-01
ifeq (3.81,$(firstword $(sort $(MAKE_VERSION) 3.81)))
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_OLDMAKEBWA_:=$(shell realpath --canonicalize-missing ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_})
else
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_OLDMAKEBWA_:=$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)
endif


ifeq (Unix,$(BuildPlatform))
LIBS	=	-lstdc++ -lc
endif


PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)include/lzma/7z.h		\
	$(PER_CONFIGURATION_BUILDS_DIR)include/lzma/7zCrc.h		\
	$(PER_CONFIGURATION_BUILDS_DIR)include/lzma/7zTypes.h

ifeq (Unix,$(BuildPlatform))
PRODUCED_OUTPUT_ARTIFACTS+=		$(PER_CONFIGURATION_BUILDS_DIR)lib/liblzma.a
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
PRODUCED_OUTPUT_ARTIFACTS+=		$(PER_CONFIGURATION_BUILDS_DIR)lib/lzma.lib
endif


#OUTPUT_WORKDIR_PRETTYNAME="{StroikaRoot}Inter.../Thir.../lzma/"
OUTPUT_WORKDIR_PRETTYNAME="$(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)")"


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
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/lzma Build $(VERSION) {$(CONFIGURATION)}:"
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
	@$(MAKE) --silent $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_OLDMAKEBWA_)
	@$(MAKE) --no-print-directory doBuilds_


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(StroikaRoot)ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)


check-prerequisite-tools:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "lzma ${VERSION} - Checking Prerequisite tools ... "
	@$(MAKE) --no-print-directory --silent check_prerequisites_
	@$(ECHO) "done"


check_prerequisites_:
	@{ type 7za > /dev/null 2>&1; } || { $(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) && $(StroikaRoot)ScriptsLib/GetMessageForMissingTool 7za && exit 1; }
	@#This weird redundant check for 7za is due to running WSL2 where the same stuff is in the path and there is a cygwin version in the path, but it wont work, so do this test which properly fails to trigger the error message about needing 7za installed under WSL2
	@{ 7za --help > /dev/null 2>&1; } || { $(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) && $(StroikaRoot)ScriptsLib/GetMessageForMissingTool 7za && exit 1; }


doBuilds_:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "lzma ${VERSION} - Build: see ${OUTPUT_WORKDIR_PRETTYNAME}BUILD-OUT.txt ..."
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) doBuildInternal_ > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)BUILD-OUT.txt 2>&1



doBuildInternal_:
ifeq (Unix,$(BuildPlatform))
	$(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) \
		AR='$(AR)' \
		CC='$(CC)' \
		CXX='$(CXX)' \
		CFLAGS='$(CFLAGS)' \
		CXXFLAGS='$(CXXFLAGS)' \
		LD='$(LINKER)' \
		LDFLAGS='$(LinkerArgs_ExtraPrefix) $(LinkerArgs_ExtraSuffix)' \
		BuildPlatform='$(BuildPlatform)' \
		LIBS='$(LIBS)' \
		all
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)/lzma.a  $(PER_CONFIGURATION_BUILDS_DIR)lib/liblzma.a
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include/lzma && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)C/7z.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)C/7zCrc.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)C/7zTypes.h $(PER_CONFIGURATION_BUILDS_DIR)include/lzma/
	cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)lzma $(PER_CONFIGURATION_BUILDS_DIR)
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)bin && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)7zDec $(PER_CONFIGURATION_BUILDS_DIR)/bin/
else
	PATH='$(TOOLS_PATH_ADDITIONS):$(PATH)';\
		$(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) \
			CC='$(CC)' \
			CXX='$(CXX)' \
			LD='$(LINKER)' \
			AR='$(LIBTOOL)' \
			BuildPlatform='$(BuildPlatform)' \
			INCLUDE='$(INCLUDES_PATH)' \
			LIBS='oleaut32.lib ole32.lib user32.lib' \
			ARCRATEFLAGS='-nologo' \
			ARTARGETPREFIX=-out: \
			LINKTARGETPREFIX=-out: \
			OBJ_SUFFIX=${OBJ_SUFFIX} LIB_SUFFIX=${LIB_SUFFIX} EXE_SUFFIX=${EXE_SUFFIX} \
			CFLAGS='$(CFLAGS)' \
			CXXFLAGS='$(CXXFLAGS)' \
			LDFLAGS='-nologo -debug -incremental:no -opt:ref $(LinkerArgs_LibPath) ' \
			all
	# Now copy the output to the BUILD directory
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)lzma.lib  $(PER_CONFIGURATION_BUILDS_DIR)lib/
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include/lzma && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)C/7z.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)C/7zCrc.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)C/7zTypes.h $(PER_CONFIGURATION_BUILDS_DIR)include/lzma/
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)bin && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)lzma.exe $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)7zDec.exe $(PER_CONFIGURATION_BUILDS_DIR)/bin/
endif



ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_OLDMAKEBWA_):	$(ZIPPEDFILE)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "lzma ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@7za x -o$@ $(ZIPPEDFILE) > /dev/null
	@if [ "$@" = "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_OLDMAKEBWA_)" ] ; then \
		cp Subdir-Makefile $@/Makefile;\
	fi


check:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "lzma ${VERSION} - Checking ... "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/lzma -    [Succeeded]";


clean:
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(StroikaRoot)Builds/*/ThirdPartyComponents/lib/{lzma}* $(StroikaRoot)Builds/*/ThirdPartyComponents/include/lzma
	@rm -rf CURRENT
else
	@rm -rf $(PRODUCED_OUTPUT_ARTIFACTS) $(PER_CONFIGURATION_BUILDS_DIR)include/lzma
endif

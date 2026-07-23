#
# One would mostly want to edit this file if there was some flag one wanted to add to CFLAGS for example, that wasn't arleady controlled
# by a value in Configuration.mk
#
# NB: Invididual makefiles will OFTEN override these values - adding INCLUDES ot the list
#
# Common bits of script / functions that maybe used in Stroika makefiles.
# This has no dependencies (besides gnu make)
#
# NB: we export variables like ECHO since they dont change from makefile to makefile and pass the values to submakes and avoid being recomputed/re-fetched(performance tweek on make)
#

#
## REMINDER ABOUT GNU MAKEFILES
#
#	In GNU Make, the assignment operators := and = define how and when variables are expanded. Specifically: 
#		:= (immediate expansion): The right-hand side of the assignment is expanded immediately when the makefile is parsed. 
#			This means any variables or functions on the right side are evaluated at that moment.
#		= (deferred expansion): The right-hand side of the assignment is not expanded until the variable is 
#			actually used in a rule or another context where it's needed. This allows variables to 
#			refer to other variables that might be defined later in the makefile. 
#


ifndef StroikaRoot
$(error("StroikaRoot must be defined and included before this file"))
endif
export StroikaRoot

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif



#
# Based on https://stackoverflow.com/questions/714100/os-detecting-makefile/52062069#52062069
#	Enumeration of common DETECTED_HOST_OS to check for if #if code
#			Cygwin
#			MSYS
#			Linux
#			Darwin
#
DETECTED_HOST_OS	:=	$(shell $(StroikaRoot)Build/ScriptsLib/DetectedHostOS)



# Needed to workaround a few MSYS issues/bugs?
ifeq (${DETECTED_HOST_OS},MSYS)
StroikaRoot_MIXED	:=	$(shell cygpath --mixed ${StroikaRoot})
else
StroikaRoot_MIXED	:=	${StroikaRoot}
endif



#
# Some makefiles - apps building stroika as a part - may define TOP_ROOT (often will). Allow Stroika makefiles to referene
# TOP_ROOT even if not in that context (just treat as alias for StroikaRoot)
#
ifndef TOP_ROOT
export TOP_ROOT	:=	$(StroikaRoot)
endif



# intentionally export just as a performance hack (avoid call to getdefaultshellvariable)
# nb we need to define ECHO at all (instead of using echo) because of some braindead default echo implementations, like AIX
export ECHO?=	$(shell $(StroikaRoot)Build/ScriptsLib/GetDefaultShellVariable ECHO)

FUNCTION_CAPITALIZE_WORD=$(shell $(ECHO) $1 | tr '[:lower:]' '[:upper:]' | cut -c 1-1)$(shell $(ECHO) $1 | cut -c 2-99)

#
# If ECHO_BUILD_LINES is set to 1, the makefile default rules will echo the build line details
#
ECHO_BUILD_LINES?=0

#
#	WRITE_PREPROCESSOR_OUTPUT if set in make commandline, then .i files written next to each .o file
#
WRITE_PREPROCESSOR_OUTPUT?=0

# intentionally export cuz sometimes MAKE_INDENT_LEVEL doesnt go up as fast as MAKELEVEL, and so submakes work out
# looking better with less explicit setting
export MAKE_INDENT_LEVEL?=$(MAKELEVEL)

# intentionally DONT set export for this cuz it could vary from makefile to makefile
SHELL?=/bin/bash

.DEFAULT_GOAL := all


#
#
#
ifndef ObjDir
	ObjDir		:=	./
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
ObjDir_ToolsSafe	=	$(shell cygpath --mixed ${ObjDir})
else
ObjDir_ToolsSafe	=	${ObjDir}
endif

#
#	This is typically the --prefix - in most third-party-component makefile/configure lines
#
ifndef StroikaPlatformTargetBuildDir
	StroikaPlatformTargetBuildDir		:=	$(StroikaRoot)Builds/$(CONFIGURATION)/
endif


#
#	StroikaLibDir
#
#		Directory where the library (.lib, or .a) Stroika libraries are found (Builds/$(CONFIGURATION)/)
#
#		Note - StroikaLibDir was StroikaPlatformTargetBuildDir (aka $(StroikaRoot)Builds/$(CONFIGURATION)/)	before Stroika 3.0d21
#		but was switched to this format to be more consistent with how pkgconfig works.
#
ifndef StroikaLibDir
	StroikaLibDir		:=	$(StroikaPlatformTargetBuildDir)lib/
endif


#
# StroikaFoundationLib and StroikaFrameworksLib are both defined for sake of dependency management, but probably
# should not be used directly in makefiles - instead depend on .pc files (as of 2025-07-03)
#
ifndef StroikaFoundationLib
ifeq (${BuildPlatform}, Unix)
	StroikaFoundationLib		:=	$(StroikaLibDir)libstroika-foundation$(LIB_SUFFIX)
else
	StroikaFoundationLib		:=	$(StroikaLibDir)stroika-foundation$(LIB_SUFFIX)
endif
endif
ifndef StroikaFrameworksLib
ifeq (${BuildPlatform}, Unix)
	StroikaFrameworksLib		:=	$(StroikaLibDir)libstroika-frameworks$(LIB_SUFFIX)
else
	StroikaFrameworksLib		:=	$(StroikaLibDir)stroika-frameworks$(LIB_SUFFIX)
endif
endif

#
#	StroikaLibs
#
# 		StroikaLibs defined for sake of dependency management, but probably
# 		should not be used directly in makefiles - instead depend on .pc files (as of 2025-07-03)
#
ifndef StroikaLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
	# NOTE - for UNIX linker - we must put libraries that depend on other libraries first
	# in the list, since the linker doesn't make multiple passes (crazy)
	StroikaLibs			:=	$(StroikaFrameworksLib) $(StroikaFoundationLib)
endif




ifndef HTMLViewCompiler
	HTMLViewCompiler	:=	"${StroikaPlatformTargetBuildDir}bin/HTMLViewCompiler"
endif


#
# Big picture point of this - is to add tools stored in non-standard locations (like some SDK subdirectory)
# into the path, so they can be invoked. We use an absolute paths for the commands we invoke (like CC)
# but these in turn sometimes invoke other things (like C-Pre-Processor) and so we must make sure they have
# their path variable setup properly to find those things (cmake maybe a better modern example).
#
# Note - before Stroika 2.1r1, we would annotate each needed cmake or cc etc line with 
# something lile:
#		PATH=${TOOLS_PATH_ADDITIONS}:${PATH})
# which also works, but this is slightly simpler, and less reliant to careful scripting
# at the point of call (bash is a shitty language).
#
# @todo if needed, could make this somehow conditional, or embed a conditional in impl of
# PATH_FOR_TOOLPATH_ADDITION_IF_NEED
#
ifneq ($(TOOLS_PATH_ADDITIONS),)
PATCH_PATH_FOR_TOOLPATH_ADDITION_IF_NEEDED=\
$(eval export PATH=$(shell cygpath --unix --path "${TOOLS_PATH_ADDITIONS}"):${shell echo $$PATH})
endif



#
# Workaround MSYS compatabilty issues with microsoft visual C++ tools.
#		Note: MSYS2_ENV_CONV_EXCL doesn't appear necessary (as of 2022-01-25)
#	    NOTE: As of 2025-06-10, this does still appear to be needed (didnt dig into why) but build of sqlite fails without it
#             on Windows with MSYS2
#
ifeq ($(DETECTED_HOST_OS),MSYS)
# See https://www.msys2.org/docs/filesystem-paths/
export MSYS2_ARG_CONV_EXCL=*
endif


#
#
#
ifeq ($(DETECTED_HOST_OS),Darwin)
SED=gsed
else
SED=sed
endif


# Common values include
#		o	stroika-platform
#		o	stroika-foundation
#		o	stroika-frameworks
#	But - define in your makefile
ifndef PackageDependencies
# will soon be no default?
PackageDependencies	:=	stroika-frameworks
endif

#
# Makefiles have 3 varaibles (by convention - not well documented anywhere)
#	CPPFLAGS	(for running c-pre-processor)
#	CFLAGS		(specific to C compiler, not including CPPFLAGS)
#	CXXFLAGS	(specific to c++ compiler, not including CPPFLAGS)
#
#	NOTE - not sure about any of these defintitions - cuz not well defined, and not well documented.
#	
#	pkgconfig - in contrast - has a single cflags element whch it uses for all of these
#
#	Use script SplitCFLAGS to split back into these three categories
#
#	\note delayed evaluation
# 		  often users will APPEND to CPPFLAGS which itself will force the evaluation
#
ifeq ($(wildcard $(StroikaLibDir)pkgconfig/stroika-frameworks.pc),)
CPPFLAGS       :=
CFLAGS         :=
CXXFLAGS       :=
else
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
# use $(shell instead of $$() for windows else fails - probably due to my not knowing how to fully enuf disable path covnersions
CPPFLAGS       =       $$(${StroikaRoot_MIXED}Build/ScriptsLib/SplitCFLAGS --type=CPPFLAGS -- $$(PKG_CONFIG_PATH="${PKG_CONFIG_PATH}" pkg-config --cflags-only-other ${PackageDependencies})) $(shell PKG_CONFIG_PATH="${PKG_CONFIG_PATH}" pkg-config --msvc --cflags-only-I ${PackageDependencies})
else
CPPFLAGS       =       $$(${StroikaRoot_MIXED}Build/ScriptsLib/SplitCFLAGS --type=CPPFLAGS -- $$(PKG_CONFIG_PATH="${PKG_CONFIG_PATH}" pkg-config --cflags-only-other ${PackageDependencies})) $$(PKG_CONFIG_PATH="${PKG_CONFIG_PATH}" pkg-config --cflags-only-I ${PackageDependencies})
endif
CFLAGS         =       $$(${StroikaRoot_MIXED}Build/ScriptsLib/SplitCFLAGS --type=CFLAGS -- $$(PKG_CONFIG_PATH="${PKG_CONFIG_PATH}" pkg-config --cflags-only-other ${PackageDependencies}))
CXXFLAGS       =       $$(${StroikaRoot_MIXED}Build/ScriptsLib/SplitCFLAGS --type=CXXFLAGS -- $$(PKG_CONFIG_PATH="${PKG_CONFIG_PATH}" pkg-config --cflags-only-other ${PackageDependencies}))
endif


#
# This macro takes two arguments:
#	$1 input src name
#	$2 OUTFILE OBJ name
#
# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
#
ifeq (Unix,$(BuildPlatform))
DEFAULT_CC_LINE=\
	"$(CC)" \
		$(CPPFLAGS) \
		$(CFLAGS) \
		-c $1 \
		-o $2
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
DEFAULT_CC_LINE=\
	"$(CC)" \
		$(CPPFLAGS) \
		$(CFLAGS) \
		-c $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1) \
		-Fo$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$2) \
		| sed -n '1!p'
endif


#
# This macro takes two arguments:
#	$1 input src name
#	$2 OUTFILE OBJ name
#
# note slightly confusingly, CPP is 'c pre processor' not c++ (which is CXX)
#
# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
#
ifeq (Unix,$(BuildPlatform))
DEFAULT_CPP_LINE=\
	"$(CXX)" \
		$(CPPFLAGS) \
		$(CXXFLAGS) \
		-E $1 \
		> $(shell dirname $2)/$(shell basename $2 .o).i
DEFAULT_CXX_LINE=\
	"$(CXX)" \
		$(CPPFLAGS) \
		$(CXXFLAGS) \
		-c $1 \
		-o $2
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
DEFAULT_CPP_LINE=\
	"$(CXX)" \
		$(CPPFLAGS) \
		$(CXXFLAGS) \
		-E $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1) \
		> $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(shell dirname $2)/$(shell basename $2 .obj)).i \
		2>&1 | sed -n '1!p'
DEFAULT_CXX_LINE=\
	"$(CXX)" \
		$(CPPFLAGS) \
		$(CXXFLAGS) \
		-c $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1) \
		-Fo$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$2) \
		| sed -n '1!p'
endif






#
# This macro (DEFAULT_LIBRARY_GEN_LINE) takes two arguments:
#	$1 OUTFILE library name
#	$2 list of OBJS
#
#	\note immediate evaluation (though would be nice todo delayed - just hard to construct that way)
#	@todo rewrite as delayed evaluation - maybe parameterize like I did for linkLine with args passed in
#
DEFAULT_LIBRARY_GEN_LINE=
ifneq ($(AR),)
DEFAULT_LIBRARY_GEN_LINE	+=	"$(AR)" cr $1 $2;
endif
ifneq ($(RANLIB),)
DEFAULT_LIBRARY_GEN_LINE	+=	"$(RANLIB)" $1
endif
ifneq ($(LIBTOOL),)
LIBTOOLFLAGS += -nologo
LIBTOOLFLAGS += -MACHINE:${WIN_LIBCOMPATIBLE_ARCH}
ifeq (-GL,$(findstring -GL,$(CXXFLAGS)))
LIBTOOLFLAGS += -LTCG
endif
ifneq ($(findstring Windows,$(TargetPlatforms)),)
# Windows now tends to run out of command-line space (depending on root dir name) - and this helps (see https://www.gnu.org/software/make/manual/html_node/File-Function.html)
DEFAULT_LIBRARY_GEN_LINE+=\
	$(shell mkdir -p `dirname $1`)\
	$(file > $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1).in,$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$2))\
	"$(LIBTOOL)" \
		-OUT:$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1) \
		${LIBTOOLFLAGS} \
		@$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1).in; \
	rm -f $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1).in
else
DEFAULT_LIBRARY_GEN_LINE+=\
	"$(LIBTOOL)" \
		-OUT:$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1) \
		${LIBTOOLFLAGS} \
		$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$2)
endif
endif


ifneq ($(LinkTime_CopyFilesToEXEDir),)
# private - dont ref DEFAULT_LINK_LINE_EXTRA_TEXT_ directly
DEFAULT_LINK_LINE_EXTRA_TEXT_ = && (cp $(LinkTime_CopyFilesToEXEDir) $(shell dirname $1) || echo "...ignored")
endif


#
# The DEFAULT_LINK_LINE macro produces a 'LINK' line from the given arguments
#	$1 is the output file name (EXE)
#	$2 [optional argument - defaults to stroika-frameworks] pkg-config dependencies for pkg-config --libs call (e.g. stroika-frameworks, stroika-foundation, or stroika-platform)
#   $3 [optional argument - defaults to empty] EXTRA LDFLAGS BEFORE OBJS
#	$4 [optional argument - defaults to empty] EXTRA LDFLAGS AFTER OBJS
#
#	\note delayed evaluation
#	\note does NOT use Platform_LDFLAGS or LDFLAGS (takes as argumements basically)
#
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
# WEIRD - need to use $(shell instead of $$() for MSYS else fails - probably due to my not knowing how to fully enuf disable path covnersions
# for MSYS and CYGWIN --LGP 2025-07-02
DEFAULT_LINK_LINE=\
	"$(LINKER)" \
		$3 \
		${OUT_ARG_PREFIX_NATIVE}$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1) \
		$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(Objs)) \
		$(shell PKG_CONFIG_PATH="${PKG_CONFIG_PATH}" ${StroikaRoot_MIXED}Build/ScriptsLib/pkg-config-msvc --static --libs $(if $2,$2,stroika-frameworks))\
		$4 \
		$(call DEFAULT_LINK_LINE_EXTRA_TEXT_, $1)
else
DEFAULT_LINK_LINE=\
	"$(LINKER)" \
		$3 \
		${OUT_ARG_PREFIX_NATIVE}$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1) \
		$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(Objs)) \
		$$(PKG_CONFIG_PATH="${PKG_CONFIG_PATH}" pkg-config --static --libs $(if $2,$2,stroika-frameworks)) \
		$4 \
		$(call DEFAULT_LINK_LINE_EXTRA_TEXT_,$1)
endif

#
#	Combination of all the LDFLAGS for the platform/basics
#	This OFTEN cannot be used directly as the flags may need to be spread out differently (see DEFAULT_LINK_LINE)
#
#	\note delayed evaluation
#
Platform_LDFLAGS	=	${Platform_LinkerArgs_ExtraPrefix} ${Platform_LinkerArgs_LibPath} ${Platform_LinkerArgs_LibDependencies} ${Platform_LinkerArgs_ExtraSuffix}


#
# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated (e.g. things added to CPPFLAGS get added to MIDL_FLAGS)
#
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
MIDL_FLAGS=		${CPPFLAGS} -nologo -W1 -char signed

DEFAULT_MIDL_LINE=\
	"$(MIDL)" \
			-iid $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$4)\
			$(MIDL_FLAGS)\
			-h $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$3) \
			-tlb $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$2) \
			-target "NT60" \
			$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1)
endif



#
# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated (e.g. things added to CPPFLAGS get added to RC_FLAGS)
#
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
RC_FLAGS	=      ${CPPFLAGS} \
						-nologo \
						-I"$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)IntermediateFiles/$(CONFIGURATION))"

DEFAULT_RC_LINE=\
	"$(RC)" \
			$(RC_FLAGS)\
			-Fo $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$2) \
			$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1)
endif


FUNCTION_QUOTE_QUOTE_CHARACTERS_FOR_SHELL=$(subst ",\",$1)

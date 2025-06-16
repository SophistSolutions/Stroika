# This file should generally not be edited. Mostly - one edits/configures the Configuration.mk, and then this generates the right
# values in make variables to be used in the SharedBuildRules.mk file.
#
# One would mostly want to edit this file if there was some flag one wanted to add to CFLAGS for example, that wasn't arleady controlled
# by a value in Configuration.mk
#
# NB: Invididual makefiles will OFTEN override these values - adding INCLUDES ot the list

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





#Common bits of script / functions that maybe used in Stroika makefiles.
#This has no dependencies (besides gnu make)

#NB: we export variables like ECHO since they dont change from makefile to makefile and pass the values to submakes and avoid being recomputed/re-fetched(performance tweek on make)

# intentionally export just as a performance hack (avoid call to getdefaultshellvariable)
# nb we need to define ECHO at all (instead of using echo) because of some braindead default echo implementations, like AIX
export ECHO?=	$(shell $(StroikaRoot)ScriptsLib/GetDefaultShellVariable ECHO)

FUNCTION_CAPITALIZE_WORD=$(shell $(ECHO) $1 | tr '[:lower:]' '[:upper:]' | cut -c 1-1)$(shell $(ECHO) $1 | cut -c 2-99)

ECHO_BUILD_LINES?=0

WRITE_PREPROCESSOR_OUTPUT?=0

# intentionally export cuz sometimes MAKE_INDENT_LEVEL doesnt go up as fast as MAKELEVEL, and so submakes work out
# looking better with less explicit setting
export MAKE_INDENT_LEVEL?=$(MAKELEVEL)

# intentionally DONT set export for this cuz it could vary from makefile to makefile
SHELL?=/bin/bash

.DEFAULT_GOAL := all


#
# Based on https://stackoverflow.com/questions/714100/os-detecting-makefile/52062069#52062069
#	Enumeration of common DETECTED_HOST_OS to check for if #if code
#			Cygwin
#			MSYS
#			Linux
#			Darwin
#
DETECTED_HOST_OS:=$(shell $(StroikaRoot)/ScriptsLib/DetectedHostOS)



### LOSE STRIP_INCLUDE_COMPILER_FLAGS DEFINE AS OBSOLETE/UNUSED as of STROIKA 3.0d21 - LGP 2025-06-10 
# ##
# ## Use this to strip /I"..." includes from a CFLAGS or CXXFLAGS list of arguments (to a windows compiler) since these
# ## sometimes cause trouble (not 100% sure why - this is a bit of a hack)
# ##
# STRIP_INCLUDE_COMPILER_FLAGS= \
# 	$(shell  echo '$1' | sed 's/[\/\-]I"[^"]*"//g')


ifndef ObjDir
	ObjDir		:=	./
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
#		Directory where the library (.lib, or .a) Stroika libraries are found (Buidlds/$(CONFIGURATION)/)
#
#		Note - StroikaLibDir was StroikaPlatformTargetBuildDir (aka $(StroikaRoot)Builds/$(CONFIGURATION)/)	before Stroika 3.0d21
#		but was switched to this format to be more consistent with how pkgconfig works.
#
ifndef StroikaLibDir
	StroikaLibDir		:=	$(StroikaPlatformTargetBuildDir)lib/
endif


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



ECHO_BUILD_LINES	?=	0


#
#	StroikaLibs
#
#		This is a space separated list of full-pathnames to the stroika library file(s)
#			PROBABLY WILL DEPRECATE IN FAVOR OF PC FILES
#
ifndef StroikaLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
	# NOTE - for UNIX linker - we must put libraries that depend on other libraries first
	# in the list, since the linker doesn't make multiple passes (crazy)
	StroikaLibs			:=	$(StroikaFrameworksLib) $(StroikaFoundationLib)
endif




ifndef HTMLViewCompiler
	HTMLViewCompiler	:=	"${StroikaPlatformTargetBuildDir}/HTMLViewCompiler"
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
# "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/HostX64/x64/cl.exe" -D_UNICODE -DUNICODE -D_WINDOWS -D_DEBUG -DqStroika_Foundation_Debug_AssertionsChecked=1 -DqStroika_HasComponent_ATLMFC=0 -DqStroika_HasComponent_boost=1 -DqStroika_HasComponent_libcurl=0 -DqStroika_HasComponent_fmtlib=0 -DqStroika_HasComponent_googletest=1 -DqStroika_HasComponent_LZMA=1 -DqStroika_HasComponent_OpenSSL=1 -DqStroika_HasComponent_sqlite=1 -DqStroika_HasComponent_mongocxxdriver=1 -DqStroika_HasComponent_WinHTTP=1 -DqStroika_HasComponent_xerces=0 -DqStroika_HasComponent_libxml2=1 -DqStroika_HasComponent_zlib=1 -DqStroika_Foundation_Debug_TraceToFile=1 -DqStroika_Foundation_Debug_DefaultTracingOn=1 -D_DISABLE_VECTOR_ANNOTATION -D_DISABLE_STRING_ANNOTATION -I"C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/include" -I"C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/ATLMFC/include" -I"C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/VS/include" -I"C:/Program Files (x86)/Windows Kits/10/include/10.0.26100.0/ucrt" -I"C:/Program Files (x86)/Windows Kits/10/include/10.0.26100.0/um" -I"C:/Program Files (x86)/Windows Kits/10/include/10.0.26100.0/shared" -I"C:/Program Files (x86)/Windows Kits/10/include/10.0.26100.0/winrt" -I"C:/Program Files (x86)/Windows Kits/10/include/10.0.26100.0/cppwinrt" -I"C:/Program Files (x86)/Windows Kits/NETFXSDK/4.6.1/include/um" -I"C:/Sandbox/Stroika/DevRoot/Builds/Debug/ThirdPartyComponents/include/" -I"C:/Sandbox/Stroika/DevRoot/Library/Sources/" -I"C:/Sandbox/Stroika/DevRoot/IntermediateFiles/Debug/" $(/c/Sandbox/Stroika/DevRoot/ScriptsLib/FixupDashIs $(pkg-config --cflags ${PKG_CONFIG_STROIKA_DEPENDS_ON})) -std:c++latest -EHsc -nologo -GR -Gd -W4 -Zc:inline -FC -bigobj -RTCsu -GS -Oy- -Od -MTd -Z7 -fsanitize=address -c Statistics.cpp -FoC:/Sandbox/Stroika/DevRoot/IntermediateFiles/Debug/Library/Foundation/Cache/Statistics.obj
# Please specify at least one package name on the command line.
# Statistics.cpp
# C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Cache\Statistics.cpp : fatal error C1083: Cannot open compiler generated file: 'C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Cache\C:\msys64\Sandbox\Stroika\DevRoot\IntermediateFiles\Debug\Library\Foundation\Cache\Statistics.obj': Invalid argument
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





# ###TMPHACK CUZ WILL BE OVERRIDEN SOON
# #;C:/Sandbox/Stroika/DevRoot/Library/Sources/;C:/Sandbox/Stroika/DevRoot/IntermediateFiles/Debug/
# ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
# X=$(shell cygpath -m ${StroikaRoot})
# else
# X=${StroikaRoot}
# endif
# CPPFLAGS += "-I${X}Library/Sources/"
# CPPFLAGS += "-I${X}IntermediateFiles/Debug/"
# @todo move to Configuration.mk (ApplyConfiguration)


# Common values include
#		o	stroika-platform
#		o	stroika-foundation
#		o	stroika-frameworks
#	But - define in your makefile
ifndef PackageDependencies
# will soon be no default?
PackageDependencies	:=	stroika-frameworks
endif

## bad - must separete out flags into CPP vs not
# use = instead of := for CPPFLAGS cuz CPPFLAGS overwritten sometimes in some makefiles and dont want to call pkg-config if inappropriate
# often users will APPEND to CPPFLAGS which itself will force the evaluation
ifeq ($(wildcard $(StroikaLibDir)pkgconfig/stroika-frameworks.pc),)
CPPFLAGS       :=
CFLAGS         :=
CXXFLAGS       :=
else
X=$(shell cygpath -m ${StroikaRoot})
CPPFLAGS       =       $$(${X}ScriptsLib/SplitCFLAGS --type=CPPFLAGS -- $$(pkg-config --cflags-only-other ${PackageDependencies})) $(shell pkg-config --msvc --cflags-only-I ${PackageDependencies})
CFLAGS         =       $$(${X}ScriptsLib/SplitCFLAGS --type=CFLAGS -- $$(pkg-config --cflags-only-other ${PackageDependencies}))
CXXFLAGS       =       $$(${X}ScriptsLib/SplitCFLAGS --type=CXXFLAGS -- $$(pkg-config --cflags-only-other ${PackageDependencies}))
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

# #EXPERIEMNTAL REPLACEMENT
# USE_CXX_FLAGS=$(shell pkg-config --msvc --cflags-only-I stroika-frameworks) $(shell pkg-config --cflags-only-other stroika-frameworks)
# # USE_CXX_FLAGS=$(shell pkg-config --cflags stroika-frameworks)
# # $(info USE_CXX_FLAGS=${USE_CXX_FLAGS})
# DEFAULT_CXX_LINE=\
# 	"$(CXX)" \
# 		${USE_CXX_FLAGS} \
# 		-c $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1) \
# 		-Fo$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$2) \
# 		| sed -n '1!p'

endif


#
# This macro takes two arguments:
#	$1 OUTFILE library name
#	$2 list of OBJS
#
# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
#
DEFAULT_LIBRARY_GEN_LINE=
ifneq ($(AR),)
DEFAULT_LIBRARY_GEN_LINE+=	"$(AR)" cr $1 $2;
endif
ifneq ($(RANLIB),)
DEFAULT_LIBRARY_GEN_LINE+=	"$(RANLIB)" $1
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


#
# LinkerArgs_StroikaDependentLibDependencies is just like LinkerArgs_LibDependencies, but
# Its for libraries that are built and dependent on Stroika.
#
# The reason for this distiction has todo with ordering (on unix linkers - not needed for windows linker).
# Dependencies in earlier libraries can be found in later, but not the other way around.
#
LinkerArgs_StroikaDependentLibDependencies :=


#
# This macro takes a single argument - the output filename for the link command
#
# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
#
DEFAULT_LINK_LINE=\
	"$(LINKER)" \
		$(LinkerArgs_ExtraPrefix) \
		$(LinkerArgs_LibPath) \
		${OUT_ARG_PREFIX_NATIVE}$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1) \
		$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(Objs)) \
		${LinkerArgs_StroikaDependentLibDependencies} \
		$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibs)) \
		$(LinkerArgs_LibDependencies) \
		$(LinkerArgs_ExtraSuffix)

# copy LinkTime_CopyFilesToEXEDir files to EXEDIR
ifneq ($(LinkTime_CopyFilesToEXEDir),)
	DEFAULT_LINK_LINE += && (cp $(LinkTime_CopyFilesToEXEDir) $(shell dirname $1) || echo "...ignored")
endif


#
# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated (e.g. things added to CPPFLAGS get added to MIDL_FLAGS)
#
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
MIDL_FLAGS=	${CPPFLAGS}
MIDL_FLAGS+=	-nologo
MIDL_FLAGS+=	-W1
MIDL_FLAGS+=	-char signed
#MIDL_FLAGS+=	-I$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)IntermediateFiles/$(CONFIGURATION))
#MIDL_FLAGS+=	-I$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)Library/Sources)

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
RC_FLAGS=	${CPPFLAGS}
RC_FLAGS+=	-nologo
RC_FLAGS+=	-I"$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)IntermediateFiles/$(CONFIGURATION))"
RC_FLAGS+=	-I"$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)Library/Sources)"

DEFAULT_RC_LINE=\
	"$(RC)" \
			$(RC_FLAGS)\
			-Fo $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$2) \
			$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1)
endif


FUNCTION_QUOTE_QUOTE_CHARACTERS_FOR_SHELL=$(subst ",\",$1)

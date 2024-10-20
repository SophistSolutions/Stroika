# This file should generally not be edited. Mostly - one edits/configures the Configuration.mk, and then this generates the right
# values in make variables to be used in the SharedBuildRules.mk file.
#
# One would mostly want to edit this file if there was some flag one wanted to add to CFLAGS for example, that wasn't arleady controlled
# by a value in Configuration.mk
#
# NB: Invididual makefiles will OFTEN override these values - adding INCLUDES ot the list


ifndef StroikaRoot
$(error("StroikaRoot must be defined and included before this file"))
endif
export StroikaRoot

ifndef ObjDir
	ObjDir		:=	./
endif


ifndef StroikaPlatformTargetBuildDir
	StroikaPlatformTargetBuildDir		:=	$(StroikaRoot)Builds/$(CONFIGURATION)/
endif


#
#	StroikaLibDir
#
#		Directory where the library (.lib, or .a) Stroika libraries are found (Buidlds/$(CONFIGURATION)/)
#
ifndef StroikaLibDir
	StroikaLibDir		:=	$(StroikaPlatformTargetBuildDir)
endif


ifndef StroikaFoundationLib
	StroikaFoundationLib		:=	$(StroikaLibDir)Stroika-Foundation$(LIB_SUFFIX)
endif
ifndef StroikaFrameworksLib
	StroikaFrameworksLib		:=	$(StroikaLibDir)Stroika-Frameworks$(LIB_SUFFIX)
endif




ECHO_BUILD_LINES	?=	0




TPP_PKG_CONFIG_PATH:=$(shell realpath --canonicalize-missing $(StroikaPlatformTargetBuildDir))/ThirdPartyComponents/lib/pkgconfig


### to make this obsolete, 2 new config arrays:
	### PKG_CONFIG_STATIC_COMPONENTS=libcurl openssl			(this we invoke pkg-config with --static...)
	### PKG_CONFIG_DYNLIB_COMPONENTS=libcurl openssl

	###AND TPP_PKG_CONFIG_PATH goes into config (as above)

#### @todo SOON MAKE THIS OBSOLETE
ifndef StroikaFoundationSupportLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
	StroikaFoundationSupportLibs	=

	ifeq ($(qFeatureFlag_LibCurl), use)
		StroikaFoundationSupportLibs += $(shell PKG_CONFIG_PATH=$(TPP_PKG_CONFIG_PATH) pkg-config --static --libs libcurl)
	endif
	ifeq ($(qFeatureFlag_LibCurl), use-system)
		StroikaFoundationSupportLibs += $(shell PKG_CONFIG_PATH=$(TPP_PKG_CONFIG_PATH) pkg-config --libs libcurl)
	endif

	ifeq ($(qFeatureFlag_OpenSSL), use)
		StroikaFoundationSupportLibs += $(shell PKG_CONFIG_PATH=$(TPP_PKG_CONFIG_PATH) pkg-config --static --libs openssl)
	endif
	ifeq ($(qFeatureFlag_OpenSSL), use-system)
		StroikaFoundationSupportLibs += $(shell PKG_CONFIG_PATH=$(TPP_PKG_CONFIG_PATH) pkg-config --libs openssl)
	endif

	StroikaFoundationSupportLibs	+=	  $(STDCPPLIBArgs)

endif


#
#	StroikaLibs
#
#		This is a space separated list of full-pathnames to the stroika library file(s)
#
ifndef StroikaLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
	# NOTE - for UNIX linker - we must put libraries that depend on other libraries first
	# in the list, since the linker doesn't make multiple passes (crazy)
	StroikaLibs					=	$(StroikaFrameworksLib) $(StroikaFoundationLib)
endif




ifndef HTMLViewCompiler
	HTMLViewCompiler	:=	"$(StroikaRoot)Builds/$(CONFIGURATION)/HTMLViewCompiler"
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
#
ifeq ($(DETECTED_HOST_OS),MSYS)
# See https://www.msys2.org/docs/filesystem-paths/
export MSYS2_ARG_CONV_EXCL=*
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
	DEFAULT_LINK_LINE += ; cp $(LinkTime_CopyFilesToEXEDir) $(shell dirname $1)
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

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
	ObjDir		=	./
endif


ifndef StroikaPlatformTargetBuildDir
	StroikaPlatformTargetBuildDir		=	$(StroikaRoot)Builds/$(CONFIGURATION)/
endif


#
#	StroikaLibDir
#
#		Directory where the library (.lib, or .a) Stroika libraries are found (Buidlds/$(CONFIGURATION)/)
#
ifndef StroikaLibDir
	StroikaLibDir		=	$(StroikaPlatformTargetBuildDir)
endif


ifndef StroikaFoundationLib
	StroikaFoundationLib		=	$(StroikaLibDir)Stroika-Foundation$(LIB_SUFFIX)
endif
ifndef StroikaFrameworksLib
	StroikaFrameworksLib		=	$(StroikaLibDir)Stroika-Frameworks$(LIB_SUFFIX)
endif




ECHO_BUILD_LINES	?=	0




TPP_PKG_CONFIG_PATH=$(shell realpath --canonicalize-missing $(StroikaPlatformTargetBuildDir))/ThirdPartyComponents/lib/pkgconfig


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



####
####
####	Expected Link Line
####		$(LINKER) $(StroikaLinkerPrefixArgs) $(OJS-and-App-Libs) -o OUTPUTFILE $(StroikaLinkerSuffixArgs)
####	
####	Stuff in StroikaLinkerPrefixArgs includes stuff like -L - directories to search, and -g, etc.
####	
####	Intentionally use '=' instead of ':=' so variables included in StroikaLinkerPrefixArgs can get re-evaluated
####
#ifeq ($(IncludeDebugSymbolsInExecutables), 1)
#	StroikaLinkerPrefixArgs += -g
#endif
### SOON TO BE DEPRECATED - LGP 2019-09-24 - use DEFAULT_LINK_LINE
ifndef StroikaLinkerPrefixArgs
	StroikaLinkerPrefixArgs	=	
endif
ifndef StroikaLinkerSuffixArgs
	StroikaLinkerSuffixArgs	=	
endif


### DONE IN CONFIGURE NOW (AND FOR A WHILE) --LGP 2019-09-20
#ifeq ($(IncludeDebugSymbolsInExecutables), 1)
#	StroikaLinkerPrefixArgs += -g
#endif
### SOON TO BE DEPRECATED - LGP 2019-09-24 - use DEFAULT_LINK_LINE
StroikaLinkerPrefixArgs+=	$(EXTRA_PREFIX_LINKER_ARGS)  $(LIBS_PATH_DIRECTIVES)



### SOON TO BE DEPRECATED - LGP 2019-09-24 - use DEFAULT_LINK_LINE
StroikaLinkerSuffixArgs	+=	$(StroikaLibs)

# (NOTE DONE FOR NOW BECAUSE SO FAR NOT NEEDED BUT HERE IS THE PLACE TO DUP LIB_DEPENDENCIES IF NEEDED): Because the linker requires libraries to go in-order, and they can have mutual dependencies, list the libraries twice
### SOON TO BE DEPRECATED - LGP 2019-09-24 - use DEFAULT_LINK_LINE
StroikaLinkerSuffixArgs+=	$(LIB_DEPENDENCIES) $(EXTRA_SUFFIX_LINKER_ARGS)

#### clean dup / do another way...StroikaFoundationSupportLibs deprecated - so just add compute of pkg-config libs) @todo -LGP 2018-05-01, -LGP 2018-05-03
### SOON TO BE DEPRECATED - LGP 2019-09-24 - use DEFAULT_LINK_LINE
StroikaLinkerSuffixArgs+=	$(StroikaFoundationSupportLibs)


ifndef HTMLViewCompiler
	HTMLViewCompiler	=	"$(StroikaRoot)Builds/$(CONFIGURATION)/HTMLViewCompiler"
endif


#
# This macro takes two arguments:
#	$1 input src name
#	$2 OUTFILE OBJ name
#
ifeq (Unix,$(ProjectPlatformSubdir))
DEFAULT_CC_LINE=\
	"$(CC)" \
		$(CFLAGS) \
		-c $1 \
		-o $2
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
DEFAULT_CC_LINE=\
	"$(CC)" \
		$(CFLAGS) \
		-c $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1) \
		/Fo$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$2) \
		| sed -n '1!p'
endif


#
# This macro takes two arguments:
#	$1 input src name
#	$2 OUTFILE OBJ name
#
ifeq (Unix,$(ProjectPlatformSubdir))
DEFAULT_CXX_LINE=\
	"$(CXX)" \
		$(CXXFLAGS) \
		-c $1 \
		-o $2
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
DEFAULT_CXX_LINE=\
	"$(CXX)" \
		$(CXXFLAGS) \
		-c $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1) \
		/Fo$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$2) \
		| sed -n '1!p'
endif



#
# This macro takes two arguments:
#	$1 OUTFILE library name
#	$2 list of OBJS
#
DEFAULT_LIBRARY_GEN_LINE=
ifneq ($(AR),)
DEFAULT_LIBRARY_GEN_LINE+=	"$(AR)" cr $1 $2;
endif
ifneq ($(RANLIB),)
DEFAULT_LIBRARY_GEN_LINE+=	"$(RANLIB)" $1
endif
ifneq ($(LIBTOOL),)
LIBTOOLFLAGS += /nologo
LIBTOOLFLAGS += /MACHINE:${WIN_LIBCOMPATIBLE_ARCH}
ifeq (/GL,$(findstring /GL,$(CXXFLAGS)))
LIBTOOLFLAGS += /LTCG
endif
DEFAULT_LIBRARY_GEN_LINE+=\
	export PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)";\
		"$(LIBTOOL)" /OUT:$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1) ${LIBTOOLFLAGS}  $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$2)
endif


#
# This macro takes a single argument - the output filename for the link command
#
DEFAULT_LINK_LINE=\
	"$(Linker)" \
		$(EXTRA_PREFIX_LINKER_ARGS) \
		$(LIBS_PATH_DIRECTIVES) \
		${OUT_ARG_PREFIX_NATIVE}$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1) \
		$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(Objs)) \
		$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibs)) \
		$(LIB_DEPENDENCIES) $(EXTRA_SUFFIX_LINKER_ARGS)



ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
MIDL_FLAGS=	${CPPFLAGS}
MIDL_FLAGS+=	/nologo
MIDL_FLAGS+=	/W1
MIDL_FLAGS+=	/char signed
#MIDL_FLAGS+=	/I$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)IntermediateFiles/$(CONFIGURATION))
#MIDL_FLAGS+=	/I$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)Library/Sources)

DEFAULT_MIDL_LINE=\
	PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)";\
		"$(MIDL)" \
			/iid $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$4)\
			$(MIDL_FLAGS)\
			/h $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$3) \
			/tlb $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$2) \
			/target "NT60" \
			$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1)
endif




ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
RC_FLAGS=	${CPPFLAGS}
RC_FLAGS+=	/nologo
RC_FLAGS+=	/I"$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)IntermediateFiles/$(CONFIGURATION))"
RC_FLAGS+=	/I"$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)Library/Sources)"

DEFAULT_RC_LINE=\
	PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)";\
		"$(RC)" \
			$(RC_FLAGS)\
			/Fo $(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$2) \
			$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$1)
endif


FUNCTION_QUOTE_QUOTE_CHARACTERS_FOR_SHELL=$(subst  ",\",$1)



ifneq (${TOOLS_PATH_ADDITIONS_BUGWORKAROUND},)
export PATH:=${TOOLS_PATH_ADDITIONS_BUGWORKAROUND}:${PATH}
endif
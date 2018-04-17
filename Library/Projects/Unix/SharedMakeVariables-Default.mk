# This file should generally not be edited. Mostly - one edits/configures the Configuration.mk, and then this generates the right
# values in make variables to be used in the SharedBuildRules.mk file.
#
# One would mostly want to edit this file if there was some flag one wanted to add to CFLAGS for example, that wasn't arleady controlled
# by a value in Configuration.mk
#
# NB: Invididual makefiles will OFTEN override these values - adding INCLUDES ot the list



ifndef StroikaRoot
	$error("StroikaRoot must be defined and included before this file (should be done automatically in Configuration.mk)")
endif
export StroikaRoot

ifndef ObjDir
	ObjDir		=	./
endif


ifndef StroikaPlatformTargetBuildDir
	StroikaPlatformTargetBuildDir		=	$(StroikaRoot)Builds/$(CONFIGURATION)/
endif

ifndef StroikaLibDir
	StroikaLibDir		=	$(StroikaPlatformTargetBuildDir)
endif


qFeatureFlag_librt				=	$(shell perl $(StroikaRoot)ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) qFeatureFlag_librt)


ifndef StroikaLinkerArgs
	StroikaLinkerArgs	= 
endif

ECHO?=	$(shell $(StroikaRoot)ScriptsLib/GetDefaultShellVariable.sh ECHO)
MAKE_INDENT_LEVEL?=$(MAKELEVEL)

ifndef Includes
	Includes	=	
endif

Includes	+=	-I$(StroikaRoot)Library/Sources/
Includes	+=	-I$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/


ifeq ($(qFeatureFlag_Xerces), 'use')
	Includes	+=  -I$(StroikaPlatformTargetBuildDir)ThirdPartyComponents/include/
else ifeq ($(qFeatureFlag_LibCurl), 'use')
	Includes	+=  -I$(StroikaPlatformTargetBuildDir)ThirdPartyComponents/include/
else ifeq ($(qFeatureFlag_LZMA), 'use')
	Includes	+=  -I$(StroikaPlatformTargetBuildDir)ThirdPartyComponents/include/
else ifeq ($(qFeatureFlag_OpenSSL), 'use')
	Includes	+=  -I$(StroikaPlatformTargetBuildDir)ThirdPartyComponents/include/
else ifeq ($(qFeatureFlag_ZLib), 'use')
	Includes	+=  -I$(StroikaPlatformTargetBuildDir)ThirdPartyComponents/include/
else ifeq ($(qFeatureFlag_sqlite), 'use')
	Includes	+=  -I$(StroikaPlatformTargetBuildDir)ThirdPartyComponents/include/
endif


IncludeDebugSymbolsInExecutables	=	$(shell perl $(StroikaRoot)/ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) IncludeDebugSymbolsInExecutables)
IncludeDebugSymbolsInLibraries		=	$(shell perl $(StroikaRoot)/ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) IncludeDebugSymbolsInLibraries)


ifndef StroikaFoundationLib
	StroikaFoundationLib		=	$(StroikaLibDir)Stroika-Foundation.a
endif
ifndef StroikaFrameworksLib
	StroikaFrameworksLib		=	$(StroikaLibDir)Stroika-Frameworks.a
endif


# Intentionally use '=' instead of ':=' so $Includes can get re-evaluated
ifndef CFLAGS
	CFLAGS		=
endif


ifndef CWARNING_FLAGS
	# These mostly don't matter, since the configure script should set this
	CWARNING_FLAGS		=	\
				-Wall \
				-Wno-switch	\
				-Wno-sign-compare \
				-Wno-unused-variable \
				-Wno-unused-but-set-variable \
				-Wno-unused-value \
				-Wno-strict-aliasing \
				-Wno-unused-local-typedefs \
				-Wno-comment
endif


#default to latest released version (except latest gcc is 4.9 and it only supports up to c++11)
# as of stroika 2.0a211 - we require at least c++14 (at least testing if this is OK)
ifndef CPPSTD_VERSION_FLAG
	#CPPSTD_VERSION_FLAG	=	--std=c++1z
	#CPPSTD_VERSION_FLAG	=	--std=c++1y
	#CPPSTD_VERSION_FLAG	=	--std=c++14
	#CPPSTD_VERSION_FLAG	=	--std=c++11
	CPPSTD_VERSION_FLAG		=	--std=c++14
endif


CFLAGS		+=	$(CPPSTD_VERSION_FLAG)  $(COPTIMIZE_FLAGS) $(Includes) $(CWARNING_FLAGS)





ECHO_BUILD_LINES	?=	0





ENABLE_GLIBCXX_DEBUG?=0




ifeq ($(STATIC_LINK_GCCRUNTIME), 1)
  ifeq ($(IF_STATIC_LINK_GCCRUNTIME_USE_PRINTPATH_METHOD), 1)
    STDCPPLIBArgs		:=      $(shell $(Linker) -print-file-name=libstdc++.a)
	ifeq ("$(wildcard $(STDCPPLIBArgs))","")
		$(error Failed to find libstdc++.a using -print-file-name - maybe turn IF_STATIC_LINK_GCCRUNTIME_USE_PRINTPATH_METHOD or apt-get install glibc-static)
	endif
  else
    STDCPPLIBArgs		:=		-lstdc++
  endif
else
  STDCPPLIBArgs			:=		-lstdc++
endif
ifeq ($(STATIC_LINK_GCCRUNTIME), 1)
	STDCPPLIBArgs	+=  -static-libstdc++
endif




TPP_PKG_CONFIG_PATH=$(shell realpath --canonicalize-missing $(StroikaPlatformTargetBuildDir))/ThirdPartyComponents/lib/pkgconfig

ifndef StroikaFoundationSupportLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
	StroikaFoundationSupportLibs	=

	#
	# Store root/--prefix installed files for third party components in Builds/$(CONFIGURATION)/ThirdPartyComponents
	#
	StroikaFoundationSupportLibs	+= -L $(StroikaPlatformTargetBuildDir)/ThirdPartyComponents/lib/

	ifneq ($(qFeatureFlag_Xerces), 'no')
		StroikaFoundationSupportLibs	+=  -lxerces-c
	endif

	ifeq ($(qFeatureFlag_LibCurl), 'use')
		StroikaFoundationSupportLibs += $(shell PKG_CONFIG_PATH=$(TPP_PKG_CONFIG_PATH) pkg-config --static --libs libcurl)
	endif
	ifeq ($(qFeatureFlag_LibCurl), 'use-system')
		StroikaFoundationSupportLibs += $(shell PKG_CONFIG_PATH=$(TPP_PKG_CONFIG_PATH) pkg-config --libs libcurl)
	endif

	ifeq ($(qFeatureFlag_LZMA), 'use')
		StroikaFoundationSupportLibs	+=  $(StroikaPlatformTargetBuildDir)ThirdPartyComponents/lib/lzma.a
	endif

	ifeq ($(qFeatureFlag_ZLib), 'use')
		StroikaFoundationSupportLibs	+=  $(StroikaPlatformTargetBuildDir)ThirdPartyComponents/lib/libz.a
	endif

	ifeq ($(qFeatureFlag_sqlite), 'use')
		StroikaFoundationSupportLibs	+=  $(StroikaPlatformTargetBuildDir)ThirdPartyComponents/lib/sqlite.a
	endif

	ifeq ($(qFeatureFlag_OpenSSL), 'use')
		StroikaFoundationSupportLibs += $(shell PKG_CONFIG_PATH=$(TPP_PKG_CONFIG_PATH) pkg-config --static --libs openssl)
	endif
	ifeq ($(qFeatureFlag_OpenSSL), 'use-system')
		StroikaFoundationSupportLibs += $(shell PKG_CONFIG_PATH=$(TPP_PKG_CONFIG_PATH) pkg-config --libs openssl)
	endif

	StroikaFoundationSupportLibs	+=	  $(STDCPPLIBArgs)

	StroikaFoundationSupportLibs	+=	  -lpthread
	ifeq ($(qFeatureFlag_librt), 'use-system')
		StroikaFoundationSupportLibs	+=	  -lrt
	endif
	StroikaFoundationSupportLibs	+=	  -lm
endif
ifndef StroikaFrameworksSupportLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
	StroikaFrameworksSupportLibs	=	
endif




ifndef StroikaLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
	# NOTE - for UNIX linker - we must put libraries that depend on other libraries first
	# in the list, since the linker doesn't make multiple passes (crazy)
	StroikaLibs					=	$(StroikaFrameworksLib) $(StroikaFoundationLib)
endif

ifndef StroikaSupportLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
	StroikaSupportLibs			=	$(StroikaFoundationSupportLibs) $(StroikaFrameworksSupportLibs)
endif
ifndef StroikaLibsWithSupportLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated

	#tmphack - added $(EXTRA_LINKER_ARGS) here as so stdlib etc come AFTER stroika libs so they get pulled in - need double listing of libs sometimes
	StroikaLibsWithSupportLibs	=	$(StroikaLibs) $(StroikaSupportLibs) $(EXTRA_LINKER_ARGS)
endif




ifndef LinkerPrefixArgs
	LinkerPrefixArgs	=	
endif



ifeq ($(ENABLE_GLIBCXX_DEBUG), 1)
	CFLAGS +=  -D_GLIBCXX_DEBUG 
endif



ifeq ($(IncludeDebugSymbolsInLibraries), 1)
	CFLAGS += -g
endif


ifeq ($(IncludeDebugSymbolsInExecutables), 1)
	LinkerPrefixArgs += -g
endif



CFLAGS	+=			$(EXTRA_COMPILER_ARGS)
LinkerPrefixArgs+=	$(EXTRA_LINKER_ARGS)

ifndef AR
	AR	= ar
endif

ifndef RANLIB
	RANLIB	= ranlib
endif

ifndef HTMLViewCompiler
	HTMLViewCompiler	=	"$(StroikaRoot)Builds/$(CONFIGURATION)/HTMLViewCompiler"
endif

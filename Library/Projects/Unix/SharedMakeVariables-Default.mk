# This file should generally not be edited. Mostly - one edits/configures the Configuration.mk, and then this generates the right
# values in make variables to be used in teh SharedBuildRules.mk file.
#
# One would mostly want to edit this file if there was some flag one wanted to add to CFLAGS for example, that wasn't arleady controlled
# by a value in Configuration.mk
#
# NB: Invididual makefiles will OFTEN override these values - adding INCLUDES ot the list, or (all of them should) override RelPathToStroikaDevRoot)




ifndef RelPathToStroikaDevRoot
	RelPathToStroikaDevRoot		=	_NO_DEFAULT_
endif


ifndef ObjDir
	ObjDir		=	./
endif


ifndef StroikaPlatformTargetBuildDir
	StroikaPlatformTargetBuildDir		=	$(RelPathToStroikaDevRoot)Builds/DefaultConfiguration/
endif

ifndef StroikaLibDir
	StroikaLibDir		=	$(StroikaPlatformTargetBuildDir)
endif


ifndef StroikaLinkerArgs
	StroikaLinkerArgs	= 
endif


ifndef Includes
	Includes	=	
endif

Includes	+=	-I$(RelPathToStroikaDevRoot)/Library/Sources/
Includes	+=	-I$(RelPathToStroikaDevRoot)/IntermediateFiles/DefaultConfiguration/


ifeq ($(qFeatureFlag_Xerces), 'use')
	Includes	+=  -I$(StroikaPlatformTargetBuildDir)ThirdPartyLibs/Includes/
else ifeq ($(qFeatureFlag_LibCurl), 'use')
	Includes	+=  -I$(StroikaPlatformTargetBuildDir)ThirdPartyLibs/Includes/
else ifeq ($(qFeatureFlag_LZMA), 'use')
	Includes	+=  -I$(StroikaPlatformTargetBuildDir)ThirdPartyLibs/Includes/
else ifeq ($(qFeatureFlag_OpenSSL), 'use')
	Includes	+=  -I$(StroikaPlatformTargetBuildDir)ThirdPartyLibs/Includes/
endif




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
	# @todo - adjust these warnings values - this was just a quick get-started...
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

	#Generates too many bogus warnings I can find no way to disable --LGP 2013-09-24
	#CWARNING_FLAGS		+=	-Wextra
	#CWARNING_FLAGS		+=	-Wno-unused-parameter
	#CWARNING_FLAGS		+=	-Wno-type-limits

	# This is a library with lots of conditional compiles. It maybe useful to get these unused-function warnings occasionally, but not generally
	CWARNING_FLAGS		+=	-Wno-unused-function

	# We run against too many different compilers, and doing they have a variety of different warning options.
	CWARNING_FLAGS		+=	-Wno-unknown-warning-option
endif


#g++-4.6 still only accepts c++0x
#default to latest released version (except latest gcc is 4.9 and it only supports up to c++11)
ifndef CPPSTD_VERSION_FLAG
	#CPPSTD_VERSION_FLAG	=	--std=c++1z
	#CPPSTD_VERSION_FLAG	=	--std=c++1y
	#CPPSTD_VERSION_FLAG	=	--std=c++14
	#CPPSTD_VERSION_FLAG	=	--std=c++11
	CPPSTD_VERSION_FLAG		=	--std=c++11
endif


CFLAGS		+=	$(CPPSTD_VERSION_FLAG)  $(COPTIMIZE_FLAGS) $(Includes) $(CWARNING_FLAGS)





ECHO_BUILD_LINES	?=	0





ifndef ENABLE_GLIBCXX_DEBUG
	ENABLE_GLIBCXX_DEBUG		=	0
endif




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






ifndef StroikaFoundationSupportLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
	StroikaFoundationSupportLibs	=

	ifeq ($(qFeatureFlag_Xerces), 'use')
		StroikaFoundationSupportLibs	+=  $(StroikaPlatformTargetBuildDir)ThirdPartyLibs/Libs/libxerces-c.a
	endif
	ifeq ($(qFeatureFlag_Xerces), 'use-system')
		StroikaFoundationSupportLibs	+=  -lxerces
	endif

	ifeq ($(qFeatureFlag_LZMA), 'use')
		StroikaFoundationSupportLibs	+=  $(StroikaPlatformTargetBuildDir)ThirdPartyLibs/Libs/lzma.a
	endif

	ifeq ($(qFeatureFlag_ZLib), 'use')
		StroikaFoundationSupportLibs	+=  $(StroikaPlatformTargetBuildDir)ThirdPartyLibs/Libs/libz.a
	endif

	ifeq ($(qFeatureFlag_OpenSSL), 'use')
		StroikaFoundationSupportLibs	+=  $(StroikaPlatformTargetBuildDir)ThirdPartyLibs/Libs/libssl.a $(StroikaPlatformTargetBuildDir)ThirdPartyLibs/Libs/libcrypto.a -ldl
	endif
	ifeq ($(qFeatureFlag_OpenSSL), 'use-system')
		StroikaFoundationSupportLibs	+=  -lopenssl -lcrypto
	endif

	ifeq ($(qFeatureFlag_LibCurl), 'use')
		# using curl-config better, but @todo - must fix ./configure script/args to point to right place to make that work
		#StroikaFoundationSupportLibs += $(shell $(StroikaPlatformTargetBuildDir)ThirdPartyLibs/curl-config --static-libs)
		StroikaFoundationSupportLibs += $(StroikaPlatformTargetBuildDir)ThirdPartyLibs/Libs/libcurl.a
	endif
	ifeq ($(qFeatureFlag_LibCurl), 'use-system')
		StroikaFoundationSupportLibs	+=  -lcurl
	endif

	StroikaFoundationSupportLibs	+=	  $(STDCPPLIBArgs)

	StroikaFoundationSupportLibs	+=	  -lpthread -lrt
	StroikaFoundationSupportLibs	+=	  -lm
endif
ifndef StroikaFrameworksSupportLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
	StroikaFrameworksSupportLibs	=	

	ifeq (AIX,$(shell uname))
		StroikaFrameworksSupportLibs	+=  -lperfstat
	endif

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
	StroikaLibsWithSupportLibs	=	$(StroikaLibs) $(StroikaSupportLibs) 
endif




ifndef LinkerPrefixArgs
	LinkerPrefixArgs	=	
endif



ifeq ($(ENABLE_GLIBCXX_DEBUG), 1)
	CFLAGS +=  -D_GLIBCXX_DEBUG 
endif



ifeq ($(INCLUDE_SYMBOLS), 1)
	CFLAGS += -g
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
	HTMLViewCompiler	=	"$(RelPathToStroikaDevRoot)Builds/DefaultConfiguration/HTMLViewCompiler"
endif

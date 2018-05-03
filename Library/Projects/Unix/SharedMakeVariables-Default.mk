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


#
#	StroikaLibDir
#
#		Directory where the library (.lib, or .a) Stroika libraries are found (Buidlds/$(CONFIGURATION)/)
#
ifndef StroikaLibDir
	StroikaLibDir		=	$(StroikaPlatformTargetBuildDir)
endif


ECHO?=	$(shell $(StroikaRoot)ScriptsLib/GetDefaultShellVariable.sh ECHO)
MAKE_INDENT_LEVEL?=$(MAKELEVEL)


ifndef StroikaFoundationLib
	StroikaFoundationLib		=	$(StroikaLibDir)Stroika-Foundation.a
endif
ifndef StroikaFrameworksLib
	StroikaFrameworksLib		=	$(StroikaLibDir)Stroika-Frameworks.a
endif


#
# Intentionally use '=' instead of ':=' so variables included in CFLAGS can get re-evaluated
#
#CPPSTD_VERSION_FLAG, COPTIMIZE_FLAGS, INCLUDES_PATH_COMPILER_DIRECTIVES, and CWARNING_FLAGS come from the included Configuration.mk file
#
ifndef CFLAGS
	CFLAGS		=
endif

CFLAGS		+=	$(CPPSTD_VERSION_FLAG)  $(COPTIMIZE_FLAGS) $(INCLUDES_PATH_COMPILER_DIRECTIVES) $(CWARNING_FLAGS)


ifeq ($(IncludeDebugSymbolsInLibraries), 1)
	CFLAGS += -g
endif
ifeq ($(ENABLE_GLIBCXX_DEBUG), 1)
	CFLAGS +=  -D_GLIBCXX_DEBUG 
endif
CFLAGS	+=			$(EXTRA_COMPILER_ARGS) $(INCLUDES_PATH)




ECHO_BUILD_LINES	?=	0

ENABLE_GLIBCXX_DEBUG?=0




#ifeq ($(STATIC_LINK_GCCRUNTIME), 1)
#  ifeq ($(IF_STATIC_LINK_GCCRUNTIME_USE_PRINTPATH_METHOD), 1)
#    STDCPPLIBArgs		:=      $(shell $(Linker) -print-file-name=libstdc++.a)
#	ifeq ("$(wildcard $(STDCPPLIBArgs))","")
#		$(error Failed to find libstdc++.a using -print-file-name - maybe turn IF_STATIC_LINK_GCCRUNTIME_USE_PRINTPATH_METHOD or apt-get install glibc-static)
#	endif
#  else
#    STDCPPLIBArgs		:=		-lstdc++
#  endif
#else
#  STDCPPLIBArgs			:=		-lstdc++
#endif
#ifeq ($(STATIC_LINK_GCCRUNTIME), 1)
#	STDCPPLIBArgs	+=  -static-libstdc++
#endif








TPP_PKG_CONFIG_PATH=$(shell realpath --canonicalize-missing $(StroikaPlatformTargetBuildDir))/ThirdPartyComponents/lib/pkgconfig


### to make this osboetel, 2 new config arrays:
	### PKG_CONFIG_STATIC_COMPONENTS=libcurl openssl			(this we invoke pkg-config with --static...)
	### PKG_CONFIG_DYNLIB_COMPONENTS=libcurl openssl

	###AND TPP_PKG_CONFIG_PATH goes into config (as above)

#### @todo SOON MAKE THIS OBSOLETE
ifndef StroikaFoundationSupportLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
	StroikaFoundationSupportLibs	=

	ifeq ($(qFeatureFlag_LibCurl), 'use')
		StroikaFoundationSupportLibs += $(shell PKG_CONFIG_PATH=$(TPP_PKG_CONFIG_PATH) pkg-config --static --libs libcurl)
	endif
	ifeq ($(qFeatureFlag_LibCurl), 'use-system')
		StroikaFoundationSupportLibs += $(shell PKG_CONFIG_PATH=$(TPP_PKG_CONFIG_PATH) pkg-config --libs libcurl)
	endif

	ifeq ($(qFeatureFlag_OpenSSL), 'use')
		StroikaFoundationSupportLibs += $(shell PKG_CONFIG_PATH=$(TPP_PKG_CONFIG_PATH) pkg-config --static --libs openssl)
	endif
	ifeq ($(qFeatureFlag_OpenSSL), 'use-system')
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



####DEPRECATD####
ifndef StroikaSupportLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
	StroikaSupportLibs			=	$(StroikaFoundationSupportLibs)
endif
####DEPRECATD####
ifndef StroikaLibsWithSupportLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated

	#tmphack - added $(EXTRA_LINKER_ARGS) here as so stdlib etc come AFTER stroika libs so they get pulled in - need double listing of libs sometimes
	StroikaLibsWithSupportLibs	=	$(LIBS_PATH_DIRECTIVES) $(StroikaLibs) $(StroikaSupportLibs) $(EXTRA_PREFIX_LINKER_ARGS) $(EXTRA_SUFFIX_LINKER_ARGS)
endif


####
####
####	Expected Link Line
####		g++ $(StroikaLinkerPrefixArgs) $(OJS-and-App-Libs) -o OUTPUTFILE $(StroikaLinkerSuffixArgs)
####	
####	Stuff in StroikaLinkerPrefixArgs includes stuff like -L - directories to search, and -g, etc.
####	
####	Intentionally use '=' instead of ':=' so variables included in StroikaLinkerPrefixArgs can get re-evaluated
####
ifndef StroikaLinkerPrefixArgs
	StroikaLinkerPrefixArgs	=	
endif
ifndef StroikaLinkerSuffixArgs
	StroikaLinkerSuffixArgs	=	
endif



ifeq ($(IncludeDebugSymbolsInExecutables), 1)
	StroikaLinkerPrefixArgs += -g
endif
StroikaLinkerPrefixArgs+=	$(EXTRA_PREFIX_LINKER_ARGS)  $(LIBS_PATH_DIRECTIVES)



StroikaLinkerSuffixArgs	+=	$(StroikaLibs)

# (NOTE DONE FOR NOW BECAUSE SO FAR NOT NEEDED BUT HERE IS THE PLACE TO DUP LIB_DEPENDENCIES IF NEEDED): Because the linker requires libraries to go in-order, and they can have mutual dependencies, list the libraries twice
StroikaLinkerSuffixArgs+=	$(LIB_DEPENDENCIES) $(EXTRA_SUFFIX_LINKER_ARGS)

#### clean dup / do another way...StroikaFoundationSupportLibs deprecated - so just add compute of pkg-config libs) @todo -LGP 2018-05-01, -LGP 2018-05-03
StroikaLinkerSuffixArgs+=	$(StroikaFoundationSupportLibs)



ifndef AR
	AR	= ar
endif

ifndef RANLIB
	RANLIB	= ranlib
endif

ifndef HTMLViewCompiler
	HTMLViewCompiler	=	"$(StroikaRoot)Builds/$(CONFIGURATION)/HTMLViewCompiler"
endif

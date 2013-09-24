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
	StroikaPlatformTargetBuildDir		=	$(RelPathToStroikaDevRoot)Builds/Platform_Linux/
endif

ifndef StroikaLibDir
	StroikaLibDir		=	$(StroikaPlatformTargetBuildDir)
endif


ifndef StroikaLinkerArgs
	StroikaLinkerArgs	= 
endif


ifndef Includes
	Includes	=	-I$(RelPathToStroikaDevRoot)/Library/Sources/
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
	#CWARNING_FLAGS		+=	\
	#			-Wextra \
	#			-Wno-unused-parameter \
	#			-Wno-type-limits
endif


#g++-4.6 still only accepts c++0x
#CFLAGS		+=	-std=c++11  $(COPTIMIZE_FLAGS) $(Includes)
CFLAGS		+=	-std=c++0x  $(COPTIMIZE_FLAGS) $(Includes) $(CWARNING_FLAGS)



ifndef ENABLE_GLIBC_ASSERTIONS
	ENABLE_GLIBC_ASSERTIONS		=	0
endif




ifeq ($(STATIC_LINK_GCCRUNTIME), 1)
  ifeq ($(IF_STATIC_LINK_GCCRUNTIME_USE_PRINTPATH_METHOD), 1)
    STDCPPLIBArgs      :=      $(shell $(Linker) -print-file-name=libstdc++.a)
  else
    STDCPPLIBArgs=		-lstdc++
    StroikaLinkerArgs	+=  -static-libstdc++
  endif
else
  STDCPPLIBArgs=		-lstdc++
endif


# -static-libgcc doesnt seem to work, nor does -static-libstdc++??? -- LGP 2011-11-07
#ifeq ($(STATIC_LINK_GCCRUNTIME), 1)
#	StroikaLinkerArgs	+=  -static-libgcc  -static-libstdc++
#endif



ifndef StroikaFoundationSupportLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
	StroikaFoundationSupportLibs	=	$(RelPathToStroikaDevRoot)ThirdPartyLibs/Xerces/CURRENT/src/.libs/libxerces-c.a  -lpthread -lrt $(STDCPPLIBArgs)
	ifeq ($(qHasFeature_libcurl), 1)
		StroikaFoundationSupportLibs	+=  -lcurl
	endif
endif
ifndef StroikaFrameworksSupportLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
	StroikaFrameworksSupportLibs	=	
endif





ifndef StroikaLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
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



ifeq ($(ENABLE_GLIBC_ASSERTIONS), 1)
	CFLAGS +=  -D_GLIBCXX_DEBUG 
endif



ifeq ($(INCLUDE_SYMBOLS), 1)
	CFLAGS += -g
endif


ifndef RANLIB
	RANLIB	= ranlib
endif



ifndef HTMLViewCompiler
	HTMLViewCompiler	=	"$(RelPathToStroikaDevRoot)Builds/Platform_Linux/HTMLViewCompiler"
endif

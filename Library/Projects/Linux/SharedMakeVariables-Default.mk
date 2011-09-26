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
	ObjDir		:=	./
endif


ifndef StroikaLibDir
	StroikaLibDir		:=	$(RelPathToStroikaDevRoot)Builds/Platform_Linux/
endif


ifndef Includes
	Includes	=	-I$(RelPathToStroikaDevRoot)/Library/Sources/
endif

ifndef StroikaFoundationLib
	StroikaFoundationLib		:=	$(StroikaLibDir)Stroika-Foundation.a
endif
ifndef StroikaFrameworksLib
	StroikaFrameworksLib		:=	$(StroikaLibDir)Stroika-Frameworks.a
endif


# Intentionally use '=' instead of ':=' so $Includes can get re-evaluated
ifndef CFLAGS
	CFLAGS		=	-c -std=c++0x -DqDebug=$(ENABLE_ASSERTIONS) $(COPTIMIZE_FLAGS) $(Includes)
endif



ifndef StroikaFoundationSupportLibs
	# Intentionally use '=' instead of ':=' so argument variables can get re-evaluated
	StroikaFoundationSupportLibs	=	$(RelPathToStroikaDevRoot)ThirdPartyLibs/Xerces/CURRENT/src/.libs/libxerces-c.a  -lpthread -lrt -lstdc++
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

### PROABBLY GET RID OF THIS NAME
#LIBS	=	$(StroikaLibsWithSupportLib)


ifeq ($(INCLUDE_SYMBOLS), 1)
	CFLAGS += -g
endif

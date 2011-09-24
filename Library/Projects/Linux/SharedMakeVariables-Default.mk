# This file should generally not be edited. Mostly - one edits/configures the Configuration.mk, and then this generates the right
# values in make variables to be used in teh SharedBuildRules.mk file.
#
# One would mostly want to edit this file if there was some flag one wanted to add to CFLAGS for example, that wasn't arleady controlled
# by a value in Configuration.mk
#
# NB: Invididual makefiles will OFTEN override these values - adding INCLUDES ot the list, or (all of them should) override RelPathToDevRoot)




RelPathToDevRoot		=	_NO_DEFAULT_
ObjDir                          =       ./
LibDir				=	$(RelPathToDevRoot)Builds/Platform_Linux/
Includes                        =       -I$(RelPathToDevRoot)/Library/Sources/


StroikaFoundationLib		=	$(LibDir)Stroika-Foundation.a
StroikaFrameworksLib		=	$(LibDir)Stroika-Frameworks.a
CFLAGS= -c -std=c++0x -DqDebug=$(ENABLE_ASSERTIONS) $(COPTIMIZE_FLAGS) $(Includes)

StroikaFoundationSupportLibs	=	$(RelPathToDevRoot)ThirdPartyLibs/Xerces/CURRENT/src/.libs/libxerces-c.a

LIBS	=	$(StroikaFrameworksLib) $(StroikaFoundationLib) $(StroikaFoundationSupportLibs) -lpthread -lrt



ifeq ($(INCLUDE_SYMBOLS), 1)
	CFLAGS += -g
endif
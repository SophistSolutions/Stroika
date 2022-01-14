#Common bits of script / functions that maybe used in Stroika makefiles which target cmake.
#
# This makefile component ASSUMES you've already included /checked:
#	$(CONFIGURATION) is Set (just skip include if not set)
#	Set StroikaRoot
#	$(CONFIGURATION)/Configuration.mk
#	Include Makefile-Common.mk
#	Include SharedMakeVariables-Default.mk
#
ifndef StroikaRoot
$(error("StroikaRoot must be defined before including Makefile-CMake-Common.mk"))
endif
ifeq ($(CONFIGURATION),)
$(error("Only include Makefile-Common.mk if CONFIGURATION is set"))
endif
ifndef FUNCTION_CAPITALIZE_WORD
$(error("be sure to include Makefile-Common.mk before including Makefile-CMake-Common.mk"))
endif
ifndef StroikaLibDir
$(error("be sure to include SharedMakeVariables-Default.mk before including Makefile-CMake-Common.mk"))
endif


####
#### Now set a bunch of CMAKE_... variables 
#### Generally set in such a way that their values are re-evaluated, so they can refer to stuff like CFLAGS
#### and still have that set later in the makefile
####


ifeq (0,$(ENABLE_ASSERTIONS))
CMAKE_CONFIGURATION:=Release
else
CMAKE_CONFIGURATION:=Debug
endif

CMAKE_PER_TARGET_BUILD_DIR:=../../Builds/$(CONFIGURATION)/ThirdPartyComponents/
CMAKE_PER_TARGET_BUILD_DIR:=$(shell realpath --canonicalize-missing $(CMAKE_PER_TARGET_BUILD_DIR) )
CMAKE_PER_TARGET_BUILD_DIR:=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(CMAKE_PER_TARGET_BUILD_DIR))

#### SEE IF OTHER GENERATORS WORK WELL/BETTER??? https://www.mankier.com/7/cmake-generators
CMAKE_USE_GENERATOR=Unix Makefiles
ifeq ($(ProjectPlatformSubdir),VisualStudio.Net-2022)
CMAKE_USE_GENERATOR=Visual Studio 17 2022
else ifeq ($(ProjectPlatformSubdir),VisualStudio.Net-2019)
CMAKE_USE_GENERATOR=Visual Studio 16 2019
else ifeq ($(ProjectPlatformSubdir),VisualStudio.Net-2017)
CMAKE_USE_GENERATOR=Visual Studio 15 2017
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
$(error "unsupported version of visual studio.net")
endif


CMAKE_ARGS=
CMAKE_ARGS+= -DCMAKE_INSTALL_PREFIX=$(CMAKE_PER_TARGET_BUILD_DIR)
ifeq (Unix,$(ProjectPlatformSubdir))
#for reasons I didn't investigate, CMAKE appears to require this to be a full path (CMAKE_C_COMPILER etc...) -- LGP 2019-01-04
CMAKE_ARGS+= -DCMAKE_C_COMPILER="$(shell which $(CC))"
CMAKE_ARGS+= -DCMAKE_CXX_COMPILER="$(shell which $(CXX))"
CMAKE_ARGS+= -DCMAKE_AR="$(shell which $(AR))"
CMAKE_ARGS+= -DCMAKE_RANLIB="$(shell which $(RANLIB))"
else
CMAKE_ARGS+= -DCMAKE_C_COMPILER="$(CC)"
CMAKE_ARGS+= -DCMAKE_CXX_COMPILER="$(CXX)"
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq ($(ProjectPlatformSubdir),VisualStudio.Net-2022)
ifeq (x86_64,$(ARCH))
CMAKE_ARGS+= -A x64
else
CMAKE_ARGS+= -A Win32
endif
else ifeq ($(ProjectPlatformSubdir),VisualStudio.Net-2019)
ifeq (x86_64,$(ARCH))
CMAKE_ARGS+= -A x64
else
CMAKE_ARGS+= -A Win32
endif
else ifeq ($(ProjectPlatformSubdir),VisualStudio.Net-2017)
ifeq (x86_64,$(ARCH))
CMAKE_USE_GENERATOR = Visual Studio 15 2017 Win64
endif
else
$(error "unsupported version of visual studio.net")
endif
endif



# For reasons not totally understood - probably just that the cmake in cygwin is quite old - it doesn't work right here.
# so use the copy from visual studio which appears to be recent enough to know about the verison of visual studio it comes with ;-)
#	--LGP 2018-12-15
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
PATH_ADJUSTMENT_PREFIX= PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)" 
endif

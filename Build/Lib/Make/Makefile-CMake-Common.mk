#Common bits of script / functions that maybe used in Stroika makefiles which target cmake.
#
# This makefile component ASSUMES you've already included /checked:
#	$(CONFIGURATION) is Set (just skip include if not set)
#	Include SharedMakeVariables-Default.mk (which in turn requires StroikaRoot to be set)
#
ifndef StroikaRoot
$(error "StroikaRoot must be defined before including Makefile-CMake-Common.mk")
endif
ifeq ($(CONFIGURATION),)
$(error "Only include Makefile-CMake-Common.mk if CONFIGURATION is set")
endif
ifndef StroikaLibDir
$(error "be sure to include SharedMakeVariables-Default.mk before including Makefile-CMake-Common.mk")
endif


$(call PATCH_PATH_FOR_TOOLPATH_ADDITION_IF_NEEDED)


####
#### Now set a bunch of CMAKE_... variables 
#### Generally set in such a way that their values are re-evaluated, so they can refer to stuff like CFLAGS
#### and still have that set later in the makefile
####


#
# Apparently under docker with MSYS2, we get a tmp and TMP and temp and TEMP environment variables set (at least under some
# circumstances). This causes a failure/crash inside the MSFT CMake (as of vs2k19 - 2022-01-19). Only seems to be an issue
# running under docker. Anyhow, this appears a safe workaround.
#
ifeq ($(DETECTED_HOST_OS),MSYS)
undefine temp
undefine tmp
endif


ifeq (0,$(AssertionsEnabled))
CMAKE_CONFIGURATION:=Release
else
CMAKE_CONFIGURATION:=Debug
endif

# CMAKE_PER_TARGET_BUILD_DIR:=${StroikaPlatformTargetBuildDir}
# CMAKE_PER_TARGET_BUILD_DIR:=$(shell realpath --canonicalize-missing $(CMAKE_PER_TARGET_BUILD_DIR) )
CMAKE_PER_TARGET_BUILD_DIR:=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaPlatformTargetBuildDir))


#
# TO DEBUG CMAKE BUILDS:
#		START WITH ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CMakeCache.txt"
#


CMAKE_ARGS=

# Verbose output to debug cmake processing
# 		https://stackoverflow.com/questions/57999508/verbose-output-for-cmake-command
# CMAKE_ARGS+= --trace
# CMAKE_ARGS+= --debug-output

# https://cmake.org/cmake/help/latest/variable/CMAKE_VERBOSE_MAKEFILE.html
# did to explore windows compiler options but didnt help with that. Leave it in for a bit
# to see if helpful for UNIX compiler options (but already not much question about those).
CMAKE_ARGS+= -DCMAKE_VERBOSE_MAKEFILE=1

## EXPERIEMNT - I THOUGHT this may fix passing -j arg to cmake submakes...but it didn't help--LGP 2025-03-19
# CMAKE_ARGS+= -DCMAKE_MAKE_PROGRAM='$(shell which $(MAKE)) ${MFLAGS} OR $(MAKEFLAGS)'
# export MFLAGS
# export MAKEFLAGS


CMAKE_ARGS+= -DCMAKE_INSTALL_PREFIX=$(CMAKE_PER_TARGET_BUILD_DIR)
ifeq (Unix,$(BuildPlatform))
#for reasons I didn't investigate, CMAKE appears to require this to be a full path (CMAKE_C_COMPILER etc...) -- LGP 2019-01-04
CMAKE_ARGS+= -DCMAKE_C_COMPILER='$(shell which $(CC))'
CMAKE_ARGS+= -DCMAKE_CXX_COMPILER='$(shell which $(CXX))'
CMAKE_ARGS+= -DCMAKE_AR='$(shell which $(AR))'
CMAKE_ARGS+= -DCMAKE_RANLIB='$(shell which $(RANLIB))'
else
CMAKE_ARGS+= -DCMAKE_C_COMPILER='$(CC)'
CMAKE_ARGS+= -DCMAKE_CXX_COMPILER='$(CXX)'
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
ifeq (x86_64,$(ARCH))
CMAKE_ARGS+= -A x64
else
CMAKE_ARGS+= -A Win32
endif
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
CMAKE_ARGS+= -DCMAKE_MSVC_RUNTIME_LIBRARY=${VSVARS_MSVC_RUNTIME_LIBRARY}
endif





#
# A bit confusing how cmake uses the _DEBUG and _RELEASE flags on windows but not unix
# No matter - set them all to the same thing
#
#	With Xerces, we get link errors if we don't include the _DEBUG and _RELEASE versions
#		-- LGP 2022-01-19
#
#	With zlib and xml2 and gtest we get:
#		libstaticd.lib(trees.obj) : warning LNK4099: PDB '' was not found with 'zlibstaticd.lib(trees.obj)' or at ''; linking object as if no debug info
#		zlibstaticd.lib(inftrees.obj) : warning LNK4099: PDB '' was not found with 'zlibstaticd.lib(inftrees.obj)' or at ''; linking object as if no debug info
#		zlibstaticd.lib(inffast.obj) : warning LNK4099: PDB '' was not found with 'zlibstaticd.lib(inffast.obj)' or at ''; linking object as if no debug info
#		xml2.lib(parserInternals.obj) : warning LNK4099: PDB '' was not found with 'xml2.lib(parserInternals.obj)' or at ''; linking object as if no debug info
#		xml2.lib(tree.obj) : warning LNK4099: PDB '' was not found with 'xml2.lib(tree.obj)' or at ''; linking object as if no debug info
#		xml2.lib(xmlsave.obj) : warning LNK4099: PDB '' was not found with 'xml2.lib(xmlsave.obj)' or at ''; linking object as if no debug info
#		gtest.lib(gtest-all.obj) : warning LNK4099: PDB '' was not found with 'gtest.lib(gtest-all.obj)' or at ''; linking
#
#	With other cmake based projects, that doesn't appear to be a problem
#
#	NOTE CMAKE_BUILD_TYPE only defined if BuildPlatform is VisualStudio
#
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
ifeq (${AssertionsEnabled},1)
CMAKE_BUILD_TYPE=Debug
CMAKE_ARGS+= -DCMAKE_C_FLAGS_DEBUG="$(PLATFORM_CPPFLAGS_NOTINCLUDES) $(CFLAGS)"
CMAKE_ARGS+= -DCMAKE_CXX_FLAGS_DEBUG="$(PLATFORM_CPPFLAGS_NOTINCLUDES) $(CXXFLAGS)"
#
#	Have cmake emit -Z7 (debug info embedded in each .obj) rather than its default -Zi (info in a
#	separate .pdb), which is what Stroika already does for its own code (see configure, and the
#	comment on WinFlag_DebugProgramDatabase for why). With -Zi, each .obj in the resulting static
#	library records a path to a .pdb, but cmake does not install compile .pdb files for static
#	library targets - so only the .lib lands in Builds/$(CONFIGURATION)/lib/ and every later link
#	against it warns, once per object file:
#		mongocxx1-static-...-mtd.lib(client.cpp.obj) : warning LNK4099: PDB 'mongocxx1-static-...-mtd.pdb' was not found ...; linking object as if no debug info
#	(losing the debug info for that third-party code). -Z7 has no such external reference, so there
#	is nothing to go missing - which is why the mongo-cxx-driver makefile no longer needs to hand-copy
#	.pdb files next to the installed libraries.
#
#	NB: an MSBuild-driven cmake build still passes -Fd, so the .obj (and hence the .lib) still has a
#	.pdb pathname recorded in it even with -Z7 - do not be alarmed by finding one. It is inert: the
#	linker does not go looking for it, and does not warn, because the debug info is already in hand.
#
#	CMAKE_MSVC_DEBUG_INFORMATION_FORMAT is only honored when CMP0141 is NEW, hence both args.
#	Mongodb does this same thing in their own CI - see, in the mongo-cxx-driver sources,
#	.evergreen/scripts/install-c-driver.sh.
#
#	Only needed/wanted for the Debug case - for Release, CMAKE_BUILD_TYPE is Release, where cmake
#	adds no debug-info flag of its own, and so whatever is in CFLAGS/CXXFLAGS already wins.
#
CMAKE_ARGS+= -DCMAKE_POLICY_DEFAULT_CMP0141=NEW
CMAKE_ARGS+= -DCMAKE_MSVC_DEBUG_INFORMATION_FORMAT=Embedded
else
CMAKE_BUILD_TYPE=Release
CMAKE_ARGS+= -DCMAKE_C_FLAGS_RELEASE="$(PLATFORM_CPPFLAGS_NOTINCLUDES) $(CFLAGS)"
CMAKE_ARGS+= -DCMAKE_CXX_FLAGS_RELEASE="$(PLATFORM_CPPFLAGS_NOTINCLUDES) $(CXXFLAGS)"
endif
else
# it appears cmake doesn't need these for windows cmake makefiles, and for mongodb-cxx-driver, it causes problems
# https://jira.mongodb.org/browse/CXX-3505
CMAKE_ARGS+= -DCMAKE_C_FLAGS="$(PLATFORM_CPPFLAGS_NOTINCLUDES) $(CFLAGS)"
CMAKE_ARGS+= -DCMAKE_CXX_FLAGS="$(PLATFORM_CPPFLAGS_NOTINCLUDES) $(CXXFLAGS)"
endif

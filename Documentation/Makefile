# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
StroikaSourceRoot=$(abspath ..)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

MAKE_INDENT_LEVEL?=$(MAKELEVEL)

INTERMEDIATE_FILES=$(StroikaIntermediateFilesRoot)Doxygen

.PHONY:	all doxygen

DOXYGEN=doxygen
# DOXYGEN=C:/tools/cygwin/bin/doxygen.exe

all:
	@#NOTE - takes a couple minutes on linux, and 55 minutes on cygwin (doxygen 1.9.2) and I think around 6 hours on MSYS doxygen (1.13.1)
	@#note sure the issue is just the doxygen version, but could be (seemed to take a while running cygwin binary on MSYS but dont recall precisely - I htink 2 hrs?)
	@#Didn't do alot of clean careful testing cuz so painful --LGP 2025-02-10
	@if [ ! -e $(StroikaBuildsRoot)Stroika-Doxygen-Help.tar.gz ] ; then\
		if which doxygen 2>&1 > /dev/null; then\
			$(MAKE) --silent doxygen;\
		else\
			$(StroikaSourceRoot)/Build/Scripts/PrintLevelLeader $(MAKE_INDENT_LEVEL) && echo "Skipping build of documentation since Doxygen not installed";\
		fi;\
	fi

clobber: clean
	@rm -f $(StroikaBuildsRoot)Stroika-Doxygen-Help.tar.gz

clean:
	@rm -rf Doxygen/Stroika-Doxygen-Help.tar.gz ${INTERMEDIATE_FILES}

doxygen:
	@$(StroikaSourceRoot)/Build/Scripts/PrintLevelLeader $(MAKE_INDENT_LEVEL) && echo -n "Building Doxygen Documentation (Builds/Stroika-Doxygen-Help.tar.gz; see IntermediateFiles/Doxygen/BUILD.txt)..."
	@rm -rf ${INTERMEDIATE_FILES}
	@mkdir -p ${INTERMEDIATE_FILES}
	@echo "PROJECT_NUMBER = \"" `$(StroikaSourceRoot)/Build/Scripts/ExtractVersionInformation $(StroikaSourceRoot)/STROIKA_VERSION FullVersionString`\" > Doxygen/PROJECT_NUMBER.cfg
	@# Stroika-Library.cfg's OUTPUT_DIRECTORY (for doxygen run by hand) overridden: a later setting wins
	@cd Doxygen && (cat Stroika-Library.cfg && echo "OUTPUT_DIRECTORY = $(shell cygpath --mixed ${INTERMEDIATE_FILES} 2>/dev/null || echo ${INTERMEDIATE_FILES})") | ${DOXYGEN} - >${INTERMEDIATE_FILES}/BUILD.txt 2>&1
	@cd ${INTERMEDIATE_FILES}; \
		mv html Stroika-Doxygen-Help; \
		tar cf Stroika-Doxygen-Help.tar Stroika-Doxygen-Help; \
		gzip --best Stroika-Doxygen-Help.tar; \
		mkdir -p $(StroikaBuildsRoot); \
		mv Stroika-Doxygen-Help.tar.gz $(StroikaBuildsRoot)
	@echo done

# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
# (its check-prerequisite-tools, clean and clobber still serve ThirdPartyComponents/Makefile's)
#
# This optional component may be used on windows to build windows installers (MSI files)
#
StroikaSourceRoot	:=	$(abspath ../..)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

### https://wixtoolset.org/docs/releasenotes/
### We are way behind here - but some work to update -  up to WIX v5 --LGP 2024-07-18

VERSION=314
FOLDERNAMEVER=wix$(VERSION)1rtm
# VERSION=311
# FOLDERNAMEVER=wix$(VERSION)2rtm
FILEVERNAME=wix$(VERSION)-binaries

ZIPPEDFILE=../Origs-Cache/$(FILEVERNAME).zip


FETCHURLS=
FETCHURLS+=https://github.com/wixtoolset/wix3/releases/download/$(FOLDERNAMEVER)/$(FILEVERNAME).zip


PRODUCED_OUTPUT_ARTIFACTS=	\
	CURRENT/candle.exe		\


all:	
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/WIX Build $(VERSION) {$(CONFIGURATION)}:"
	@$(StroikaSourceRoot)Build/Scripts/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --silent CURRENT
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check

clean:

clobber:
	@rm -rf CURRENT

$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(StroikaSourceRoot)Build/Scripts/WebGet $(ZIPPEDFILE) $(FETCHURLS)

CURRENT:	$(ZIPPEDFILE)
	@unzip -q $(ZIPPEDFILE) -d CURRENT
	@/bin/find CURRENT -name \*.exe -exec chmod +x {} \;
	@/bin/find CURRENT -name \*.dll -exec chmod +x {} \;

check-prerequisite-tools:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "WIX $(VERSION) - Checking Prerequisite tools ... "
	@{ type unzip > /dev/null; } || { $(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Checking required build tools ... " && $(StroikaSourceRoot)Build/Scripts/GetMessageForMissingTool unzip && exit 1; }
	@$(ECHO) "done";

check:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "WIX $(VERSION) - Checking ... "
	@$(StroikaSourceRoot)Build/Scripts/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/WIX -  [Succeeded]";

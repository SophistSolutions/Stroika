#
# This optional component may be used on windows to build windows installers (MSI files)
#
StroikaRoot:=$(abspath ../..)/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

### https://wixtoolset.org/docs/releasenotes/
### We are way behind here - but some work to update -  up to WIX v5 --LGP 2024-07-18

VERSION=311
FOLDERNAMEVER=wix$(VERSION)2rtm
#FOLDERNAMEVER=wix$(VERSION)1rtm
FILEVERNAME=wix$(VERSION)-binaries

ZIPPEDFILE=../Origs-Cache/$(FILEVERNAME).zip


FETCHURLS=
FETCHURLS+=https://github.com/wixtoolset/wix3/releases/download/$(FOLDERNAMEVER)/$(FILEVERNAME).zip


PRODUCED_OUTPUT_ARTIFACTS=	\
	CURRENT/candle.exe		\


all:	
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/WIX Build $(VERSION) {$(CONFIGURATION)}:"
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --silent CURRENT
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check

clean:

clobber:
	@rm -rf CURRENT

$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(StroikaRoot)ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)

CURRENT:	$(ZIPPEDFILE)
	@unzip -q $(ZIPPEDFILE) -d CURRENT
	@/bin/find CURRENT -name \*.exe -exec chmod +x {} \;
	@/bin/find CURRENT -name \*.dll -exec chmod +x {} \;

check-prerequisite-tools:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "WIX $(VERSION) - Checking Prerequisite tools ... "
	@{ type unzip > /dev/null; } || { $(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Checking required build tools ... " && $(StroikaRoot)ScriptsLib/GetMessageForMissingTool unzip && exit 1; }
	@$(ECHO) "done";

check:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "WIX $(VERSION) - Checking ... "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/WIX -  [Succeeded]";

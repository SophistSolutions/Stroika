#
# This optional component may be used on windows to build windows installers (MSI files)
#
export StroikaRoot?=$(shell realpath ../..)/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

VERSION=311
FOLDERNAMEVER=wix$(VERSION)1rtm
FILEVERNAME=wix$(VERSION)-binaries

ZIPEDFILE=../Origs-Cache/$(FILEVERNAME).zip


FETCHURLS=
FETCHURLS+=https://github.com/wixtoolset/wix3/releases/download/$(FOLDERNAMEVER)/$(FILEVERNAME).zip


all:	
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/WIX Build $(VERSION) {$(CONFIGURATION)}:"
	@../../ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(MAKE) --silent CURRENT

clean:

clobber:
	@rm -rf CURRENT

$(ZIPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ../../ScriptsLib/WebGet.sh $(ZIPEDFILE) $(FETCHURLS)

CURRENT:	$(ZIPEDFILE)
	@unzip -q $(ZIPEDFILE) -d CURRENT


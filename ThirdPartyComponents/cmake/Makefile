#
#	Xerces 3.2 and later requires cmake to build on windows, and the cygwin version
#	doesnt work with building for windows (at least not easily?), so keep a binary around.
#
#
MAJ_VERSION=3.10
VERSION=$(MAJ_VERSION).0-rc3
BASENAME=cmake-$(VERSION)-win32-x86
#BASENAME=cmake-$(VERSION)-win64-x64
ZIPPEDFILE=../Origs-Cache/$(BASENAME).zip

TARGET_TAR=$(shell pwd)/../Origs-Cache/$(VER).tar
TARGET_ZIP=$(TARGET_TAR).gz
EXTRACT_FROM=/cygdrive/c/Perl/

export StroikaRoot=$(shell realpath ../..)/

SHELL=/bin/bash

MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ../../ScriptsLib/GetDefaultShellVariable.sh ECHO)



FETCHURLS=
FETCHURLS+=https://cmake.org/files/v$(MAJ_VERSION)/$(BASENAME).zip



all:	
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika/ThirdPartyComponents/cmake (windows/cygwin) $(VERSION):"
	@$(MAKE) --silent CURRENT

clean:

clobber:
	@rm -rf CURRENT


extract:	CURRENT

$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ../../ScriptsLib/WebGet.sh $(ZIPPEDFILE) $(FETCHURLS)

CURRENT:	$(ZIPPEDFILE)
	@rm -rf CURRENT
	@unzip -q $(ZIPPEDFILE)
	@mv $(BASENAME) CURRENT
	@###@todo find better way so no need to post-process and make binaries executable (dlls)
	@chmod +x CURRENT/bin/*
	@touch CURRENT

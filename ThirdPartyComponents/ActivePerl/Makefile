#
#	OpenSSL 1.1.0f and later requires ActivePerl to build on windows, so keep
#	a binary around.
#
#	To 'capture-current' - install the desired version of ActivePerl (and update VER below).
#	And run make-current
#	Checkin the created .tar.gz in ../Origs-Cache
#	Then you can uninstall perl
#
export StroikaRoot?=$(shell realpath ../..)/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

VERSION=5.26.0
VER=ActivePerl-$(VERSION)-MSWin32-x86


TARGET_TAR=../Origs-Cache/$(VER).tar
TARGET_ZIP=$(TARGET_TAR).gz
EXTRACT_FROM=/cygdrive/c/Perl/


all:	
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/ActivePerl Build $(VERSION) {$(CONFIGURATION)}:"
	@../../ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(MAKE) --silent CURRENT

clean:

clobber:
ifeq ($(CONFIGURATION),)
	@rm -rf CURRENT
endif

capture-current:
	@cd $(EXTRACT_FROM) && tar --create --file $(TARGET_TAR) *
	@gzip --best $(TARGET_TAR)


extract:	CURRENT


CURRENT:
	@rm -rf CURRENT
	@mkdir CURRENT
	@tar xf $(TARGET_ZIP) --directory CURRENT

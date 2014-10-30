.DEFAULT_GOAL := all
.PHONY:	all clean clobber check

MIRROR_PREFIX_=http://psg.mtu.edu/pub/apache/
VERSION=3.1.1
BASENAME=xerces-c-$(VERSION)
ZIPPEDFILE=../Origs-Cache/$(BASENAME).tar.gz
TARFILE=$(BASENAME).tar
EXTRACTED_DIRNAME=$(BASENAME)
SLINKDIRNAME=$(BASENAME)
USE_DIRNAME=CURRENT

TRGDIR=CURRENT

FETCHURL=$(MIRROR_PREFIX_)/xerces/c/3/sources/$(BASENAME).tar.gz

UNAME := $(shell uname)



all:
	@echo "   ...Building Stroika/ThirdPartyLibs/xerces VERSION=$(VERSION)..."
	@make --no-print-directory doCreate
	@make --no-print-directory doBuild

clean:
	@rm -rf $(EXTRACTED_DIRNAME) CURRENT

check:
	@./checkall.pl

clobber:
	@rm -rf $(EXTRACTED_DIRNAME) CURRENT

$(ZIPPEDFILE):
	@echo -n "   ..."
	wget --quiet --tries=10 --no-check-certificate -O $(ZIPPEDFILE) $(FETCHURL) || (rm -f $(ZIPPEDFILE) && false)


doCreate:	$(ZIPPEDFILE)
	@if [ -e  CURRENT/src/xercesc/dom/impl/DOMLocatorImpl.hpp ]; then\
		echo "   ...CURRENT dir already exists";\
	else\
		echo "   ...Creating $(TRGDIR)...";\
		rm -rf $(USE_DIRNAME) CURRENT;\
		tar xf $(ZIPPEDFILE) --no-same-owner;\
		mv $(EXTRACTED_DIRNAME) $(TRGDIR);\
		#ln -s CURRENT $(SLINKDIRNAME);\
		echo "   ...Patching Xerces...";\
		(cd CURRENT; tar xf ../Patches/VC11Projects.tar.gz --no-same-owner);\
		(cd CURRENT; tar xf ../Patches/VC12Projects.tar.gz --no-same-owner);\
		chmod --recursive +rwx CURRENT/projects;\
	fi

doBuild:
	@echo "   ...Building REDIRECT to BUILD.OUT..."
	@./buildall_.pl build > BUILD.OUT 2>&1

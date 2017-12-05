### SUPER ROUGH EXPERIMENTAL DRAFT OF BOOST SUPPORT
ifneq ($(CONFIGURATION),)
ProjectPlatformSubdir	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
endif

SHELL=/bin/bash

.DEFAULT_GOAL := all
.PHONY:	all clean clobber check doBuild_

MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ../../ScriptsLib/GetDefaultShellVariable.sh ECHO)



-include $(shell pwd)/../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

VERSION=1_65_1


all:	CURRENT
	$(MAKE) doBuild_


FETCHURLS=
FETCHURLS+=https://dl.bintray.com/boostorg/release/1.65.1/source/boost_$(VERSION).tar.gz


boost_$(VERSION).tar.gz:
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ../../ScriptsLib/WebGet.sh $(ZIPPEDFILE) $(FETCHURLS)
	

boost_$(VERSION): boost_$(VERSION).tar.gz
	tar xf boost_$(VERSION).tar.gz

CURRENT:	boost_$(VERSION)
	mv boost_$(VERSION) CURRENT


doBuild_:
	
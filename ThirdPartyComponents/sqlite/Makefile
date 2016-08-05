ifneq ($(CONFIGURATION),)
ProjectPlatformSubdir	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
endif

.DIRECTORY: CURRENT

VERSION=3130000
AMALGAMATION_WITH_VERSION=sqlite-amalgamation-$(VERSION)

ZIPEDFILE=../Origs-Cache/$(AMALGAMATION_WITH_VERSION).zip

FETCHURLS=
FETCHURLS+=https://www.sqlite.org/2016/$(AMALGAMATION_WITH_VERSION).zip



.DEFAULT_GOAL := all
.PHONY:	all clean clobber check doBuild doCreate doPatch

.NOTPARALLEL:


MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ../../ScriptsLib/GetDefaultShellVariable.sh ECHO)


$(ZIPEDFILE):
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1))
	@../../ScriptsLib/WebGet.sh $(ZIPEDFILE) $(FETCHURLS)


all:	CURRENT

clobber:
	@rm -rf sqlite-$(AMALGAMATION_WITH_VERSION) CURRENT
	
clean:
	@rm -rf $(AMALGAMATION_WITH_VERSION)

CURRENT:	$(ZIPEDFILE)
	@unzip -q $(ZIPEDFILE)
	@mv $(AMALGAMATION_WITH_VERSION) CURRENT
	@touch CURRENT

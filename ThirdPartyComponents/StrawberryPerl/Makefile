#
#	This is only used for building openssl, because the perl that comes with msys/cygwin somehow doesn't
#	work for building openssl. Tried ActivePerl for a while but too buggy/PITA to install
#
#	To run STRAWBERRYPERL (roughly):
#		PATH=$(StroikaRoot)ThirdPartyComponents/StrawberryPerl/CURRENT/perl/bin:$PATH		OR
#		portableshell.bat from 'CURRENT' directory
#
StroikaRoot:=$(abspath ../..)/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk


#
# CHECK FOR UPDATED VERSIONS https://strawberryperl.com/releases.html
#	MAYBE BETTER https://github.com/StrawberryPerl/Perl-Dist-Strawberry/releases
#
PERL_VERSION_=5.38.2.2
#PERL_VERSION_=5.32.1.1

ZIPPED_SOURCES_FILE_=../Origs-Cache/strawberry-perl-${PERL_VERSION_}-64bit-portable.zip

FETCHURLS_=

FETCHURLS_+=https://github.com/StrawberryPerl/Perl-Dist-Strawberry/releases/download/SP_53822_64bit/strawberry-perl-${PERL_VERSION_}-64bit-portable.zip
#FETCHURLS_+=https://strawberryperl.com/download/${PERL_VERSION_}/strawberry-perl-${PERL_VERSION_}-64bit-portable.zip


${ZIPPED_SOURCES_FILE_}:
	@MAKE_INDENT_LEVEL=$$((${MAKE_INDENT_LEVEL}+1)) ${StroikaRoot}ScriptsLib/WebGet ${ZIPPED_SOURCES_FILE_} ${FETCHURLS_}

check-prerequisite-tools:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "strawberry-perl $(PERL_VERSION_) - Checking Prerequisite tools ... "
	@{ type unzip > /dev/null; } || { $(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Checking required build tools ... " && $(StroikaRoot)ScriptsLib/GetMessageForMissingTool unzip && exit 1; }
	@$(ECHO) "done";

ORIGs CURRENT:	$(ZIPPED_SOURCES_FILE_)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "strawberry-perl ${PERL_VERSION_} - Extracting $(ZIPPED_SOURCES_FILE_) to $@"` "... "
	mkdir -p $@
	unzip -q $(ZIPPED_SOURCES_FILE_) -d $@


PRODUCED_OUTPUT_ARTIFACTS_=	\
	${StroikaRoot}ThirdPartyComponents/StrawberryPerl/CURRENT		\
	${StroikaRoot}ThirdPartyComponents/StrawberryPerl/CURRENT/perl/bin/perl.exe	\

all:	
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/strawberry-perl Build $(PERL_VERSION_):"
	@$(MAKE) --silent CURRENT
	@${MAKE} --no-print-directory MAKE_INDENT_LEVEL=$$((${MAKE_INDENT_LEVEL}+1)) check


check:
ifeq (${CONFIGURATION},)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@${StroikaRoot}ScriptsLib/PrintProgressLine ${MAKE_INDENT_LEVEL} -n "strawberry-perl ${libxml2_VERSION_} - Checking ... "
	@${StroikaRoot}ScriptsLib/CheckFileExists ${PRODUCED_OUTPUT_ARTIFACTS_}
	@${ECHO} " Stroika/ThirdPartyComponents/strawberry-perl -    [Succeeded]";


clean:


clobber:
ifeq ($(CONFIGURATION),)
	@rm -rf CURRENT
endif

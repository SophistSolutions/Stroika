#
#	some perl besides cygwin/msys required to build openssl. Tried ActivePerl for a while but too buggy/
#	PITA to install
#
#	To run STRAWBERRYPERL (roughly):
#		PATH=$(StroikaRoot)ThirdPartyComponents/StrawberryPerl/CURRENT/perl/bin:$PATH		OR
#		portableshell.bat from 'CURRENT' directory
#
StroikaRoot=$(abspath ../..)/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

VERSION=5.32.1.1

#
# CHECK FOR UPDATED VERSIONS https://strawberryperl.com/releases.html
#
PERL_VERSION_=5.32.1.1

ZIPPED_SOURCES_FILE_=../Origs-Cache/strawberry-perl-${PERL_VERSION_}-64bit-portable.zip

FETCHURLS_=
FETCHURLS_+=https://strawberryperl.com/download/${PERL_VERSION_}/strawberry-perl-${PERL_VERSION_}-64bit-portable.zip


${ZIPPED_SOURCES_FILE_}:
	@MAKE_INDENT_LEVEL=$$((${MAKE_INDENT_LEVEL}+1)) ${StroikaRoot}ScriptsLib/WebGet ${ZIPPED_SOURCES_FILE_} ${FETCHURLS_}


ORIGs CURRENT:	$(ZIPPED_SOURCES_FILE_)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "strawberry-perl ${PERL_VERSION_} - Extracting $(ZIPPED_SOURCES_FILE_) to $@"` "... "
	mkdir -p $@
	unzip -q $(ZIPPED_SOURCES_FILE_) -d $@


all:	
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/ActivePerl Build $(VERSION):"
	@$(MAKE) --silent CURRENT


clean:


clobber:
ifeq ($(CONFIGURATION),)
	@rm -rf CURRENT
endif

#!/usr/bin/perl 


my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

require ("../../ScriptsLib/BuildUtils.pl");
require ("../../ScriptsLib/StringUtils.pl");

#print "PATH=";
#print (" $ENV{'PATH'};");

#both seem to work (2013-04-06) - but test with old for regressions first cuz new build
# process -- LGP 2013-04-06 (part of stroika instead of RFLLib)
#my $BASENAME	=	"openssl-1.0.0";
my $BASENAME	=	"openssl-1.0.1e";

my $EXTRACTED_DIRNAME	=	$BASENAME;
my $trgDirName	=			$BASENAME;
my $SLINKDIRNAME	=		$BASENAME;

# DoCreateSymLink - this isn't currently used, and doesn't work well with windows (sets sys file attribute which casues
# DOS delete file issues) - and slink doesnt really work except in cygwin tools
my $DoCreateSymLink = 0;

if ("$^O" eq "linux") {
	$DoCreateSymLink = 1;
}


print (">>>>>>>>******************** STARTING ThirdPartyLibs/openssl ******************\n");

if ((lc ("$BLD_TRG") eq "clean") || (lc ("$BLD_TRG") eq "clobber")) {
	system ("rm -rf $trgDirName CURRENT");
	exit (0);
}
if (lc ("$BLD_TRG") eq "rebuild") {
	system ("rm -rf $trgDirName CURRENT");
}

if (not -e "../Origs-Cache/$BASENAME.tar.gz") {
	RunAndStopOnFailure ("wget --quiet --tries=10 --no-check-certificate --output-document=../Origs-Cache/$BASENAME.tar.gz http://www.openssl.org/source/$BASENAME.tar.gz");
}

print ("Extracting $BASENAME...\n");
if (not -e "CURRENT/e_os.h") {
	system ("rm -rf $trgDirName CURRENT");
	system ("tar xf ../Origs-Cache/$BASENAME.tar.gz 2> /dev/null");
	sleep(1);  # hack cuz sometimes it appears command not fully done writing - and we get sporadic failures on next stop on win7
	system ("mv $EXTRACTED_DIRNAME CURRENT");
	sleep(1);  # hack cuz sometimes it appears command not fully done writing - and we get sporadic failures on next stop on win7
	if ($DoCreateSymLink) {
		system ("ln -s CURRENT $SLINKDIRNAME");
	}
	
	print ("Patching openssl...\n");
	#system ("patch -t CURRENT/e_os.h Patches/e_os.h.patch");
}



sub	CopyBuilds2Out
{
	my $trgDir = $_[0];
	my $bldOutDir = $_[1];
	system ("mkdir -p CURRENT/Builds/$trgDir");
	system ("cp -r CURRENT/inc32/ CURRENT/Builds/$trgDir/Includes");
	system ("cp CURRENT/$bldOutDir/libeay32.* CURRENT/Builds/$trgDir/");
	system ("cp CURRENT/$bldOutDir/ssleay32.* CURRENT/Builds/$trgDir/");
	system ("cp CURRENT/$bldOutDir/openssl.* CURRENT/Builds/$trgDir/");
}


system ("rm -rf CURRENT/Builds");

#REM - only reconfigure if we just did the extract
if ("$^O" eq "linux") {
	#NB: we disable ICO and CURL because these gave some problem with Gentoo (link error), and
	#	not worth tracking down further cuz I don't think we need either -- LGP 2011-09-27
	system ("cd CURRENT ; ./config -no-shared");
	system ("cd CURRENT ; make -s all");
}
else {if ("$^O" eq "cygwin") {
	chdir ("CURRENT");
		system ("perl Configure VC-WIN32 no-asm --prefix=c:/some/openssl/dir");
		## BASED ON ms\do_ms.bat
		system ("perl util/mkfiles.pl >MINFO");
		system ("perl util/mk1mf.pl no-asm VC-WIN32 >ms/nt.mak");
		system ("perl util/mk1mf.pl debug no-asm VC-WIN32 >ms/nt-DBG.mak");
		system ("perl util/mkdef.pl 32 libeay > ms/libeay32.def");
		system ("perl util/mkdef.pl 32 ssleay > ms/ssleay32.def");
		system ("rm -f NUL");
		system ("rm -rf tmp32 tmp32.dbg out32 out32.dbg");	# cuz changing proj files might not have right depends

		print (" ...Make Release...\n");
		#nb: lose -s to see each compile line
		system ("(nmake /NOLOGO /S -f ms/nt.mak 2>&1) | tee -a NT.MAK.BUILD-Output.txt");

		print (" ...Make Debug...\n");
		#nb: lose -s to see each compile line
		system ("(nmake /NOLOGO /S -f ms/nt-DBG.mak 2>&1) | tee -a NT-DBG.MAK.BUILD-Output.txt");

		print (" ...Running openssl tests...");
		system ("(nmake /NOLOGO /S -f ms/nt.mak test 2>&1) > TEST-OUT.txt");
		system ("(nmake /NOLOGO /S -f ms/nt-DBG.mak test 2>&1) > TEST-DBG-OUT.txt");
		print ("\n");
	chdir ("..");

	CopyBuilds2Out ("Release32", "out32");
	CopyBuilds2Out ("Debug32", "out32.dbg");

	chdir ("CURRENT");
		system ("perl Configure VC-WIN64I no-asm --prefix=c:/some/openssl/dir");
		## BASED ON ms\do_win64i.bat
		system ("perl util/mkfiles.pl >MINFO");
		system ("perl ms/uplink-ia64.pl > ms/uptable.asm");
		system ("ias -o ms/uptable.obj ms/uptable.asm");
		system ("perl util/mk1mf.pl VC-WIN64I >ms/nt.mak");
		system ("perl util/mk1mf.pl debug VC-WIN64I >ms/nt-DBG.mak");
		system ("perl util/mkdef.pl 32 libeay > ms/libeay32.def");
		system ("perl util/mkdef.pl 32 ssleay > ms/ssleay32.def");
		system ("rm -f NUL");
		system ("rm -rf tmp32 tmp32.dbg out32 out32.dbg");	# cuz changing proj files might not have right depends

		print (" ...Make Release64...\n");
		#nb: lose -s to see each compile line
		system ("(nmake /NOLOGO /S -f ms/nt.mak 2>&1) | tee -a NT64.MAK.BUILD-Output.txt");

		print (" ...Make Debug64...\n");
		#nb: lose -s to see each compile line
		system ("(nmake /NOLOGO /S -f ms/nt-DBG.mak 2>&1) | tee -a NT64-DBG.MAK.BUILD-Output.txt");

		print (" ...Running openssl tests...");
		system ("(nmake /NOLOGO /S -f ms/nt.mak test 2>&1) > TEST-OUT.txt");
		system ("(nmake /NOLOGO /S -f ms/nt-DBG.mak test 2>&1) > TEST-DBG-OUT.txt");
		print ("\n");
	chdir ("..");

	CopyBuilds2Out ("Release64", "out32");
	CopyBuilds2Out ("Debug64", "out32.dbg");

}}



system ("perl checkall.pl");

print (">>>>>>>>******************** ENDING ThirdPartyLibs/openssl ******************\n");

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

if (-e "CURRENT/e_os.h") {
	print ("already up to date\n");
	goto DONE;
}

if (not -e "../Origs-Cache/$BASENAME.tar.gz") {
	print ("wget --quiet --no-check-certificate --output-document=../Origs-Cache/$BASENAME.tar.gz http://www.openssl.org/source/$BASENAME.tar.gz ...\n");
	system ("wget --quiet --no-check-certificate --output-document=../Origs-Cache/$BASENAME.tar.gz http://www.openssl.org/source/$BASENAME.tar.gz");
}

print ("Extracting $BASENAME...\n");
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


sub RunAndPrint
{
	my $cmd2Run = $_[0];
	print ("$cmd2Run...\n");
	my $result = system ($cmd2Run);
	if ($result != 0) {
		print "Run result = $result\r\n";
	}
}


#REM - only reconfigure if we just did the extract
print ("Configurating openssl...\n");
if ("$^O" eq "linux") {
	#NB: we disable ICO and CURL because these gave some problem with Gentoo (link error), and
	#	not worth tracking down further cuz I don't think we need either -- LGP 2011-09-27
	system ("cd CURRENT ; ./config -no-shared");
}
else {if ("$^O" eq "cygwin") {
	chdir ("CURRENT");
		system ("perl Configure VC-WIN32 no-asm --prefix=c:/some/openssl/dir");
		## FROM ms\do_ms.bat (with one line skipped - dont bother with DLL)
		system ("perl util/mkfiles.pl >MINFO");
		system ("perl util/mk1mf.pl no-asm VC-WIN32 >ms/nt.mak");
		#system ("perl util\mk1mf.pl dll no-asm VC-WIN32 >ms/ntdll.mak");
		system ("perl util/mkdef.pl 32 libeay > ms/libeay32.def");
		system ("perl util/mkdef.pl 32 ssleay > ms/ssleay32.def");
		
		###extras
		# Build 32-bit DEBUG library
		system ("perl util/mk1mf.pl debug no-asm VC-WIN32 >ms/nt-DBG.mak");
		
		system ("rm -f NUL");
	chdir ("..");
}}

print ("Building openssl...\n");
if ("$^O" eq "linux") {
	system ("cd CURRENT ; make -s all");
}
else {if ("$^O" eq "cygwin") {
	chdir ("CURRENT");

###NOT FULLY WORKING - SHOULD BUILD MOST STUFF SO NOT NEEDED REBUILD
		#this trick make line is just to make build more quiet - it can be elimianted and makes all
		#To get rid of it - just delete the 2 make lines that use this define
		if (1) {
			print (" Make Build (first  quietly - output saved to .txt file)...");
			my $JUNK_MUST_BLD_TO_GET_SILENT_BUT_SUCCESSFUL_BUILD = "tmp32 inc32 out32 inc32/openssl headers lib";
			#my $JUNK_MUST_BLD_TO_GET_SILENT_BUT_SUCCESSFUL_BUILD = "";
			system ("(nmake /NOLOGO -f ms/nt.mak $JUNK_MUST_BLD_TO_GET_SILENT_BUT_SUCCESSFUL_BUILD 2>&1) >> NT.MAK.BUILD-Output.txt");
			system ("(nmake /NOLOGO -f ms/nt-DBG.mak $JUNK_MUST_BLD_TO_GET_SILENT_BUT_SUCCESSFUL_BUILD 2>&1) >> NT-DBG.MAK.BUILD-Output.txt");
		}
		
		print (" ...Make Release...\n");
		#nb: lose -s to see each compile line
		system ("(nmake /NOLOGO -s -f ms/nt.mak 2>&1) | tee -a NT.MAK.BUILD-Output.txt");

		print (" ...Make Debug...\n");
		#nb: lose -s to see each compile line
		system ("(nmake /NOLOGO -s -f ms/nt-DBG.mak 2>&1) | tee -a NT-DBG.MAK.BUILD-Output.txt");
		
		print (" ...Running openssl tests...");
		system ("(nmake /NOLOGO -s -f ms/nt.mak test 2>&1) > TEST-OUT.txt");
		system ("(nmake /NOLOGO -s -f ms/nt-DBG.mak test 2>&1) > TEST-DBG-OUT.txt");
		print ("\n");
	chdir ("..");
}}

system ("perl checkall.pl");

DONE:
print (">>>>>>>>******************** ENDING ThirdPartyLibs/openssl ******************\n");


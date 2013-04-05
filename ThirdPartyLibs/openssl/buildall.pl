#!/usr/bin/perl 


my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

require ("../../ScriptsLib/BuildUtils.pl");

#print "PATH=";
#print (" $ENV{'PATH'};");

my $BASENAME	=	"openssl-1.0.1e";
#my $BASENAME	=	"openssl-1.0.0";

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
	exit (0);
}

require "../../ScriptsLib/ConfigurationReader.pl";
print ("Extracting openssl...\n");

system ("rm -rf $trgDirName CURRENT");
system ("tar xf Origs/$BASENAME.tar.gz 2> /dev/null");
sleep(1);  # hack cuz sometimes it appears command not fully done writing - and we get sporadic failures on next stop on win7
system ("mv $EXTRACTED_DIRNAME CURRENT");
sleep(1);  # hack cuz sometimes it appears command not fully done writing - and we get sporadic failures on next stop on win7
if ($DoCreateSymLink) {
	system ("ln -s CURRENT $SLINKDIRNAME");
}

print ("Patching openssl...\n");
system ("patch -t CURRENT/e_os.h Patches/e_os.h.patch");


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

chdir ("CURRENT");
	#system ("perl Configure VC-WIN32 nasm --prefix=c:\some\openssl\dir");
	system ("perl Configure VC-WIN32 nasm");
	system ("perl util/mkfiles.pl >MINFO");
	system ("perl util/mk1mf.pl nasm VC-WIN32 >ms/nt.mak");
	system ("perl util/mkdef.pl 32 libeay > ms/libeay32.def");
	system ("perl util/mkdef.pl 32 ssleay > ms/ssleay32.def");
	system ("perl util/mk1mf.pl nasm VC-WIN32 >ms/nt.mak");
	system ("perl util/mk1mf.pl debug nasm VC-WIN32 >ms/nt-DBG.mak");
chdir ("..");



if ("$^O" eq "linux") {
	print ("Configurating openssl...\n");
	#NB: we disable ICO and CURL because these gave some problem with Gentoo (link error), and
	#	not worth tracking down further cuz I don't think we need either -- LGP 2011-09-27
	system ("cd CURRENT ; ./configure --enable-static --disable-shared --without-icu --without-curl");
}

system ("rm -f NUL");


print ("Building openssl...\n");
if ("$^O" eq "linux") {
	system ("cd CURRENT ; make -s all");
}
else {
	chdir ("CURRENT");
		print (" Make Release Build... (first  quietly - output saved to .txt file)");
		system ("nmake /NOLOGO -s -f ms/nt.mak banner");
		system ("nmake /NOLOGO -s -f ms/nt.mak %JUNK_MUST_BLD_TO_GET_SILENT_BUT_SUCCESSFUL_BUILD% >> NT.MAK.BUILD-Output.txt");
		system ("nmake /NOLOGO -s -f ms/nt.mak");
		
		print (" Make Debug Build... (first  quietly - output saved to .txt file)");
		system ("nmake /NOLOGO -s -f ms/nt-DBG.mak banner");
		system ("nmake /NOLOGO -s -f ms/nt-DBG.mak %JUNK_MUST_BLD_TO_GET_SILENT_BUT_SUCCESSFUL_BUILD% >> NT-DBG.MAK.BUILD-Output.txt");
		system ("nmake /NOLOGO -s -f ms/nt-DBG.mak");
		
		print (" running openssl tests...");
		system ("(nmake /NOLOGO -s -f ms/nt.mak test 2>&1) > TEST-OUT.txt");
		system ("(nmake /NOLOGO -s -f ms/nt-DBG.mak test 2>&1) > TEST-DBG-OUT.txt");
	chdir ("..");
}

system ("perl checkall.pl");

DONE:
print (">>>>>>>>******************** ENDING ThirdPartyLibs/openssl ******************\n");


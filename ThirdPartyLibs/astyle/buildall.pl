#!/usr/bin/perl 


my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}


my $BASENAME	=	"astyle-trunk-2013-02-25";

my $EXTRACTED_DIRNAME	=	"astyle-trunk";
my $trgDirName	=			$BASENAME;
my $SLINKDIRNAME	=		$BASENAME;

# DoCreateSymLink - this isn't currently used, and doesn't work well with windows (sets sys file attribute which casues
# DOS delete file issues) - and slink doesnt really work except in cygwin tools
my $DoCreateSymLink = 0;

if ("$^O" eq "linux") {
	$DoCreateSymLink = 1;
}


print (">>>>>>>>******************** STARTING ThirdPartyLibs/astyle ******************\n");

if ((lc ("$BLD_TRG") eq "clean") || (lc ("$BLD_TRG") eq "clobber")) {
	system ("rm -rf $trgDirName CURRENT");
	exit (0);
}
if (lc ("$BLD_TRG") eq "rebuild") {
	system ("rm -rf $trgDirName CURRENT");
}

if (-e "CURRENT/AStyle/build/gcc/bin/astyle") {
	print ("already up to date\n");
	goto DONE;
	exit (0);
}
if (-e "CURRENT/AStyle/build/gcc/bin/astyle.exe") {
	print ("already up to date\n");
	goto DONE;
	exit (0);
}

require "../../ScriptsLib/ConfigurationReader.pl";
print ("Extracting astyle...\n");

system ("rm -rf $trgDirName CURRENT");
system ("tar xf ../Origs-Cache/$BASENAME.tar.gz 2> /dev/null");
sleep(1);  # hack cuz sometimes it appears command not fully done writing - and we get sporadic failures on next stop on win7
system ("mv $EXTRACTED_DIRNAME CURRENT");
sleep(1);  # hack cuz sometimes it appears command not fully done writing - and we get sporadic failures on next stop on win7
if ($DoCreateSymLink) {
	system ("ln -s CURRENT $SLINKDIRNAME");
}



sub RunAndPrint
{
	my $cmd2Run = $_[0];
	print ("$cmd2Run...\n");
	my $result = system ($cmd2Run);
	if ($result != 0) {
		print "Run result = $result\r\n";
	}
}




print ("Building astyle...\n");
system ("cd CURRENT/AStyle/build/gcc/ ; make all");

system ("perl checkall.pl");

DONE:
print (">>>>>>>>******************** ENDING ThirdPartyLibs/astyle ******************\n");


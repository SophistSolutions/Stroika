#!/usr/bin/perl

require "../../ScriptsLib/ConfigurationReader.pl";

my $myPlatformSubDir =	GetProjectPlatformSubdir ();

my $XXX;

if ("$^O" eq "linux") {
	#
	#for now seems to build just one lib...
	#
	$XXX = "CURRENT/AStyle/build/gcc/bin/astyle";
}
else {
	$XXX = "CURRENT/AStyle/build/gcc/bin/astyle.exe";
}

if (! (-e "$XXX")) {
	print ("Echo [FAILED] - $XXX is missing\n");
	exit (1);
}
print ("ThirdPartyLibs/astyle -  [SUCCEEDED]\n");

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
	my $myBinOutDir = '';
	if ($myPlatformSubDir eq 'VisualStudio.Net-2010') {
		$myBinOutDir = 'VC10';
	}
	if ($myPlatformSubDir eq 'VisualStudio.Net-2012') {
		$myBinOutDir = 'VC11';
	}
	$XXX = "CURRENT/Build/Win32/$myBinOutDir/Static Release/xerces-c_static_3.lib";
}

if (! (-e "$XXX")) {
	print ("Echo [FAILED] - $XXX is missing\n");
	exit (1);
}
print ("ThirdPartyLibs/astyle -  [SUCCEEDED]\n");

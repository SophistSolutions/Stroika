#!/usr/bin/perl

#### NOTE - DONT CALL DIRECTLY - LEGACY CODE FROM OLD MAKE PROCESS TO BE MERGED
#### INTO MAKEFILE

require "../../ScriptsLib/ConfigurationReader.pl";

my $myPlatformSubDir =	GetProjectPlatformSubdir ();

my $DBG32_LIB;
my $REL32_LIB;
my $DBG64_LIB;
my $REL64_LIB;

print "   ...Checking...";

if (("$^O" eq "linux") or ("$^O" eq "darwin")) {
	#
	#for now seems to build just one lib...
	#
	$REL32_LIB = "CURRENT/src/.libs/libxerces-c.a";
	$DBG32_LIB = $REL32_LIB;
	$DBG64_LIB = $REL32_LIB;
	$REL64_LIB = $REL32_LIB;
}
else {
	my $myBinOutDir = '';
	if ($myPlatformSubDir eq 'VisualStudio.Net-2010') {
		$myBinOutDir = 'VC10';
	}
	if ($myPlatformSubDir eq 'VisualStudio.Net-2012') {
		$myBinOutDir = 'VC11';
	}
	if ($myPlatformSubDir eq 'VisualStudio.Net-2013') {
		$myBinOutDir = 'VC12';
	}
	$REL32_LIB = "CURRENT/Build/Win32/$myBinOutDir/Static Release/xerces-c_static_3.lib";
	$DBG32_LIB = "CURRENT/Build/Win32/$myBinOutDir/Static Debug/xerces-c_static_3D.lib";
	$REL64_LIB = "CURRENT/Build/Win64/$myBinOutDir/Static Release/xerces-c_static_3.lib";
	$DBG64_LIB = "CURRENT/Build/Win64/$myBinOutDir/Static Debug/xerces-c_static_3D.lib";
}

if (! (-e "$REL32_LIB")) {
	print ("      ThirdPartyLibs/Xerces - [FAILED] - $REL32_LIB is missing\n");
	exit (1);
}
if (! (-e "$DBG32_LIB")) {
	print ("      ThirdPartyLibs/Xerces - [FAILED] - $DBG32_LIB is missing\n");
	exit (1);
}
if (! (-e "$REL64_LIB")) {
	print ("      ThirdPartyLibs/Xerces - [FAILED] - $REL64_LIB is missing\n");
	exit (1);
}
if (! (-e "$DBG64_LIB")) {
	print ("      ThirdPartyLibs/Xerces - [FAILED] - $DBG64_LIB is missing\n");
	exit (1);
}
print ("      ThirdPartyLibs/Xerces -  [SUCCEEDED]\n");

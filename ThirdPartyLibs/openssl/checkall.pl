#!/usr/bin/perl

require "../../ScriptsLib/ConfigurationReader.pl";

my $myPlatformSubDir =	GetProjectPlatformSubdir ();

my $libeay32;
my $libeay32_DBG;
my $ssleay32;
my $ssleay32_DBG;

if ("$^O" eq "linux") {
	###WRONG - MUST FIX
	$libeay32 = "CURRENT/out32/libeay32.lib";
	$libeay32_DBG = "CURRENT/out32.dbg/libeay32.lib";
	$ssleay32 = "CURRENT/out32/ssleay32.lib";
	$ssleay32_DBG = "CURRENT/out32.dbg/ssleay32.lib";
}
else {
	$libeay32 = "CURRENT/out32/libeay32.lib";
	$libeay32_DBG = "CURRENT/out32.dbg/libeay32.lib";
	$ssleay32 = "CURRENT/out32/ssleay32.lib";
	$ssleay32_DBG = "CURRENT/out32.dbg/ssleay32.lib";
}

if (! (-e "$libeay32")) {
	print ("Echo [FAILED] - $libeay32 is missing\n");
	exit (1);
}
if (! (-e "$libeay32_DBG")) {
	print ("Echo [FAILED] - $libeay32_DBG is missing\n");
	exit (1);
}
if (! (-e "$ssleay32")) {
	print ("Echo [FAILED] - $ssleay32 is missing\n");
	exit (1);
}
if (! (-e "$ssleay32_DBG")) {
	print ("Echo [FAILED] - $ssleay32_DBG is missing\n");
	exit (1);
}
print ("ThirdPartyLibs/openssl -  [SUCCEEDED]\n");

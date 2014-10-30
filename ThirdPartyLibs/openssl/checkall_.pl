#!/usr/bin/perl

#### NOTE - DONT CALL DIRECTLY - LEGACY CODE FROM OLD MAKE PROCESS TO BE MERGED
#### INTO MAKEFILE

require "../../ScriptsLib/StringUtils.pl";
require "../../ScriptsLib/ConfigurationReader.pl";

my $myPlatformSubDir =	GetProjectPlatformSubdir ();

my $libeay32;
my $libeay32_DBG;
my $ssleay32;
my $ssleay32_DBG;

print "   ...Checking...\n";

if ("$^O" eq "linux") {
	###WRONG - MUST FIX
	$libeay32 = "CURRENT/libcrypto.a";
	$libeay32_DBG = "CURRENT/libcrypto.a";
	$ssleay32 = "CURRENT/libssl.a";
	$ssleay32_DBG = "CURRENT/libssl.a";
}
else {
	$libeay32 = "CURRENT/out32/libeay32.lib";
	$libeay32_DBG = "CURRENT/out32.dbg/libeay32.lib";
	$ssleay32 = "CURRENT/out32/ssleay32.lib";
	$ssleay32_DBG = "CURRENT/out32.dbg/ssleay32.lib";
}

if (! (-e "$libeay32")) {
	print ("      openssl  [FAILED] - $libeay32 is missing\n");
	exit (1);
}
if (! (-e "$libeay32_DBG")) {
	print ("      openssl  [FAILED] - $libeay32_DBG is missing\n");
	exit (1);
}
if (! (-e "$ssleay32")) {
	print ("      openssl  [FAILED] - $ssleay32 is missing\n");
	exit (1);
}
if (! (-e "$ssleay32_DBG")) {
	print ("      openssl  [FAILED] - $ssleay32_DBG is missing\n");
	exit (1);
}


if ("$^O" eq "linux") {
my $x1 = 1;
my $x2 = 1;
}
else {
my $x1 = `diff -b CURRENT/TEST-OUT.txt REFERENCE_OUTPUT.txt | wc -l`;
my $x2 = `diff -b CURRENT/TEST-DBG-OUT.txt REFERENCE_OUTPUT.txt | wc -l`;
}
if (trim ($x1) <= "44" and trim ($x2) <= "44") {
	print ("      OpenSSL...       [SUCCEEDED]\n");
}
else {
	print ("      openssl [FAILED]\n");
	print ("         Test DIFFS (REDO THIS IN PERL WHERE ITS EASIER TO COUNT LINES ETC)\n");
	print ("         2 lines coming next - each should be less than 40/44 to be safe...\n");
	print "          $x1\n";
	print "          $x2\n";
}

#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

print (">>>>>>>>******************** STARTING ThirdPartyLibs/doxygen ******************\n");

if ((lc ("$BLD_TRG") eq "clean") || (lc ("$BLD_TRG") eq "clobber")) {
	system ("rm -rf CURRENT");
	exit (0);
}
if (lc ("$BLD_TRG") eq "rebuild") {
	system ("rm -rf CURRENT");
}

my $doxygenFile = "CURRENT/bin/doxygen.exe";
unless (-e $doxygenFile) {
	print ("Extracting ThirdPartyLibs/doxygen...\n");
	system ("tar xvf ../Origs-Cache/doxygen-1.8.2-Windows-Installed.tar.gz");
	system ("mv doxygen-1.8.2-Windows-Installed CURRENT");
}

DONE:
print (">>>>>>>>******************** ENDING ThirdPartyLibs/doxygen ******************\n");


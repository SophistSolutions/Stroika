#!/usr/bin/perl

if (! (-e "CURRENT/bin/doxygen.exe")) {
	print ("Echo [FAILED] - CURRENT/bin/doxygen.exe is missing\n");
	exit (1);
}
print ("ThirdPartyLibs/doxygen - [SUCCEEDED]\n");

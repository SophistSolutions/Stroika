#!/usr/bin/perl 

require "../Scripts/checkallHelpers.pl";

if (("$^O" eq "linux") or ("$^O" eq "darwin")) {
	# nothing to check - not supported on Linux
}
else {
	CheckFileExists ("../../Builds/Debug-U-32/Samples-ArchiveUtility/ArchiveUtility.exe");
	CheckFileExists ("../../Builds/Release-U-32/Samples-ArchiveUtility/ArchiveUtility.exe");
}
print "[SUCCEEDED]\r\n";

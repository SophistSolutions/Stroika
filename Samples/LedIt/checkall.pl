#!/usr/bin/perl 

require "../Scripts/checkallHelpers.pl";

if ("$^O" eq "linux") {
	# nothing to check - not supported on Linux
}
else {
	CheckFileExists ("../../Builds/Debug-U-32/Samples-LedIt/LedIt.exe");
	CheckFileExists ("../../Builds/Release-U-32/Samples-LedIt/LedIt.exe");
}
print "[SUCCEEDED]\r\n";

#!/usr/bin/perl 

require "../Scripts/checkallHelpers.pl";

if (("$^O" eq "linux") or ("$^O" eq "darwin")) {
	# nothing to check - not supported on Linux
}
else {
	CheckFileExists ("../../Builds/Debug-U-32/Samples-SystemPerformanceClient/SystemPerformanceClient.exe");
	CheckFileExists ("../../Builds/Release-U-32/Samples-SystemPerformanceClient/SystemPerformanceClient.exe");
}
print "[SUCCEEDED]\r\n";

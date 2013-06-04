#!/usr/bin/perl 

require "../Scripts/checkallHelpers.pl";

if ("$^O" eq "linux") {
	# nothing to check - not supported on Linux
}
else {
	CheckFileExists ("../../Builds/Windows/Debug-U-32/Samples-SampleService/SampleService.exe");
	CheckFileExists ("../../Builds/Windows/Release-U-32/Samples-SampleService/SampleService.exe");
}
print "[SUCCEEDED]\r\n";

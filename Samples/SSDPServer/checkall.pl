#!/usr/bin/perl 

require "../Scripts/checkallHelpers.pl";

if (("$^O" eq "linux") or ("$^O" eq "darwin")) {
	# nothing to check - not supported on Linux
}
else {
	CheckFileExists ("../../Builds/Windows/Debug-U-32/Samples-SSDPServer/SSDPServer.exe");
	CheckFileExists ("../../Builds/Windows/Release-U-32/Samples-SSDPServer/SSDPServer.exe");
}
print "[SUCCEEDED]\r\n";

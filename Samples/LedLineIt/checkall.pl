#!/usr/bin/perl 

require "../Scripts/checkallHelpers.pl";

my $activeConfig = $ENV{'CONFIGURATION'};

if ("$^O" eq "linux") {
	# nothing to check - not supported on Linux
}
else {
	if ($activeConfig eq "Debug-U-32" || $activeConfig eq "Release-U-32") {
		CheckFileExists ("../../Builds/$activeConfig/Samples-LedLineIt/LedLineIt.exe");
	}
	else {
		print "[SKIPPED]\r\n";
		exit(0);
	}
}
print "[SUCCEEDED]\r\n";

#!/usr/bin/perl 

require "../Scripts/checkallHelpers.pl";

my $activeConfig = $ENV{'CONFIGURATION'};
my $projectPlatformSubdir = `../../ScriptsLib/PrintConfigurationVariable.pl $activeConfig ProjectPlatformSubdir`;

if ($projectPlatformSubdir eq "Unix") {
	# nothing to check - not supported on Linux
}
else {
	if ($activeConfig eq "Debug-U-32" || $activeConfig eq "Release-U-32" || $activeConfig eq "Debug-U-64" || $activeConfig eq "Release-U-64") {
		CheckFileExists ("../../Builds/$activeConfig/Samples-SSDPServer/SSDPServer.exe");
	}
	else {
		print "[SKIPPED]\r\n";
		exit(0);
	}
}
print "[SUCCEEDED]\r\n";

#!/usr/bin/perl 

require "../Scripts/checkallHelpers.pl";

my $activeConfig = $ENV{'CONFIGURATION'};
my $projectPlatformSubdir = `../../ScriptsLib/PrintConfigurationVariable.pl $activeConfig ProjectPlatformSubdir`;

if ($projectPlatformSubdir eq "Unix") {
	# nothing to check - not supported on Linux
}
else {
	CheckFileExists ("../../Builds/$activeConfig/Samples-ArchiveUtility/ArchiveUtility.exe");
}
print "[SUCCEEDED]\r\n";

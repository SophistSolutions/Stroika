#!/usr/bin/perl 

require "../../ScriptsLib/ConfigurationReader.pl";
require "../../ScriptsLib/BuildUtils.pl";

# these environment flags appear to confuse nmake
delete $ENV{'MAKEFLAGS'};

my $activeConfig = $ENV{'CONFIGURATION'};
my $projectPlatformSubdir = GetProjectPlatformSubdir ($activeConfig);
my $useBld = NormalizeBuildArg ($ARGV[0]);
my $useProjectDir= "Projects/" . $projectPlatformSubdir;
my $level = $ENV{'MAKE_INDENT_LEVEL'};

# these environment flags appear to confuse nmake
delete $ENV{'MAKEFLAGS'};

my $curConfig	=	`../../ScriptsLib/GetVisualStudioConfigLine.pl $activeConfig`;


print(`../../ScriptsLib/PrintLevelLeader.sh $level` . "Building Samples/ActiveLedIt...\n");
if (index($projectPlatformSubdir, "VisualStudio") != -1) {
	my $extraArgs = GetMSBuildArgs();
	if ($activeConfig eq "Debug-U-32" || $activeConfig eq "Release-U-32") {
		RunAndPrint ("cd $useProjectDir; msbuild.exe $extraArgs ActiveLedIt.vcxproj /p:$curConfig /target:$useBld");
	}
}

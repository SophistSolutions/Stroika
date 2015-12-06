#!/usr/bin/perl 

require "../../ScriptsLib/ConfigurationReader.pl";
require "../../ScriptsLib/BuildUtils.pl";

my $activeConfig = $ENV{'CONFIGURATION'};
my $projectPlatformSubdir = GetProjectPlatformSubdir ($activeConfig);
my $useBld = NormalizeBuildArg ($ARGV[0]);
my $useProjectDir= "Projects/" . $projectPlatformSubdir;
my $level = $ENV{'MAKE_INDENT_LEVEL'};


print(`../../ScriptsLib/PrintLevelLeader.sh $level` . "Building Samples/LedLineIt...\n");
if ($activeConfig eq "Debug-U-32" || $activeConfig eq "Release-U-32") {
	my $curConfig	=	`../../ScriptsLib/GetVisualStudioConfigLine.pl $activeConfig`;
	my $extraArgs = GetMSBuildArgs();
	RunAndPrint ("cd $useProjectDir; msbuild.exe $extraArgs LedLineIt.vcxproj /p:$curConfig /target:$useBld");
}

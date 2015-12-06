#!/usr/bin/perl 

require "../../ScriptsLib/ConfigurationReader.pl";
require "../../ScriptsLib/BuildUtils.pl";

my $activeConfig = $ENV{'CONFIGURATION'};;
my $projectPlatformSubdir = GetProjectPlatformSubdir($activeConfig);
my $useProjectDir= "Projects/" . $projectPlatformSubdir;
my $level = $ENV{'MAKE_INDENT_LEVEL'};

my $useBld = NormalizeBuildArg ($ARGV[0]);


# these environment flags appear to confuse nmake
delete $ENV{'MAKEFLAGS'};


print(`../../ScriptsLib/PrintLevelLeader.sh $level` . "Building Samples/ArchiveUtility...\n");
if ($activeConfig eq "Debug-U-32" || $activeConfig eq "Release-U-32" || $activeConfig eq "Debug-U-64" || $activeConfig eq "Release-U-64" || $activeConfig eq "Release-Logging-U-64" || $activeConfig eq "Release-DbgMemLeaks-U-32") {
	my $curConfig	=	`../../ScriptsLib/GetVisualStudioConfigLine.pl $activeConfig`;
	my $extraArgs = GetMSBuildArgs();
	RunAndPrint ("cd $useProjectDir; msbuild.exe $extraArgs ArchiveUtility.vcxproj /p:$curConfig /target:$useBld");
}

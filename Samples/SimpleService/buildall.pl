#!/usr/bin/perl 

require "../../ScriptsLib/ConfigurationReader.pl";
require "../../ScriptsLib/BuildUtils.pl";

# these environment flags appear to confuse nmake
delete $ENV{'MAKEFLAGS'};

my $activeConfig = $ENV{'CONFIGURATION'};
my $projectPlatformSubdir = GetProjectPlatformSubdir ($activeConfig);
my $useBld = NormalizeBuildArg ($ARGV[0]);
my $useProjectDir= "Projects/" . $projectPlatformSubdir;
my $level = $ENV{'MAKELEVEL'};



print(`../../ScriptsLib/PrintLevelLeader.sh $level` . "Building Samples/SimpleService...\n");
if (index($projectPlatformSubdir, "VisualStudio") == -1) {
	use Cwd;
	use Cwd 'abs_path';
	my $savedDir = abs_path (getcwd ());
	chdir ("../../IntermediateFiles/$activeConfig/");
	if ($useBld eq "Build") {
		$useBld = "all";
	}
	system ("cd Samples_SimpleService; make -s $useBld");
	chdir ($savedDir);
}
else {
	if ($activeConfig eq "Debug-U-32" || $activeConfig eq "Release-U-32" || $activeConfig eq "Debug-U-64" || $activeConfig eq "Release-U-64") {
		my $curConfig	=	`../../ScriptsLib/GetVisualStudioConfigLine.pl $activeConfig`;
		my $extraArgs = GetMSBuildArgs();
		RunAndPrint ("cd $useProjectDir; msbuild.exe $extraArgs SimpleService.vcxproj /p:$curConfig /target:$useBld");
	}
}

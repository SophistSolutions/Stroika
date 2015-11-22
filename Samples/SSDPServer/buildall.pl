#!/usr/bin/perl 

require "../../ScriptsLib/ConfigurationReader.pl";
require "../../ScriptsLib/BuildUtils.pl";

my $activeConfig = "DefaultConfiguration";
my $projectPlatformSubdir = GetProjectPlatformSubdir($activeConfig);
my $useProjectDir= "Projects/" . $projectPlatformSubdir;

my $useBld = NormalizeBuildArg ($ARGV[0]);

my @kConfigurations = (
					"Configuration=Debug-U-32,Platform=Win32",
					"Configuration=Debug-U-64,Platform=x64",
					"Configuration=Release-U-32,Platform=Win32",
					"Configuration=Release-U-64,Platform=x64",
					);


print("   Building Samples/SSDPServer...\n");
if (index($projectPlatformSubdir, "VisualStudio") == -1) {
	use Cwd;
	use Cwd 'abs_path';
	my $savedDir = abs_path (getcwd ());

	chdir ("../../IntermediateFiles/$activeConfig");
	if ($useBld eq "rebuild") {
		print ("Samples_SSDPServer; clobber...\n");
		system ("cd Samples_SSDPServer; make -s clobber");
		$useBld = "all";
	}
	
	system ("cd Samples_SSDPServer; make -s $useBld");
	chdir ($savedDir);
}
else {
	foreach (@kConfigurations) {
		my $curConfig	=	$_;
		my $extraArgs = GetMSBuildArgs();
		RunAndPrint ("cd $useProjectDir; msbuild.exe $extraArgs SSDPServer.vcxproj /p:$curConfig /target:$useBld");
	}
}

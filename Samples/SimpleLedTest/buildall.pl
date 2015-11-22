#!/usr/bin/perl 

require "../../ScriptsLib/ConfigurationReader.pl";
require "../../ScriptsLib/BuildUtils.pl";

my $projectPlatformSubdir = GetProjectPlatformSubdir ("DefaultConfiguration");
my $useBld = NormalizeBuildArg ($ARGV[0]);
my $useProjectDir= "Projects/" . $projectPlatformSubdir;

my @kConfigurations = (	
					"Configuration=Debug-U-32,Platform=Win32",
					"Configuration=Release-U-32,Platform=Win32",
					);


print("   Building Samples/SimpleLedTest...\n");
if (index($projectPlatformSubdir, "VisualStudio") != -1) {
	foreach (@kConfigurations) {
		my $curConfig	=	$_;
		my $extraArgs = GetMSBuildArgs();
		RunAndPrint ("cd $useProjectDir; msbuild.exe $extraArgs SimpleLedTest.vcxproj /p:$curConfig /target:$useBld");
	}
}

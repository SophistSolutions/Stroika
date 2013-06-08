#!/usr/bin/perl 

require "../../ScriptsLib/ConfigurationReader.pl";
require "../../ScriptsLib/BuildUtils.pl";

my $useBld = NormalizeBuildArg ($ARGV[0]);

my @kConfigurations = (	
					"Configuration=Debug-A-32,Platform=Win32",
					"Configuration=Release-A-32,Platform=Win32",
					"Configuration=Debug-U-32,Platform=Win32",
					"Configuration=Debug-U-64,Platform=x64",
					"Configuration=Release-U-32,Platform=Win32",
					"Configuration=Release-U-64,Platform=x64",
					);

my $useProjectDir= "Projects/" . GetProjectPlatformSubdir ();

print("Building Samples/SampleService...\n");
if ("$^O" eq "linux") {
	use Cwd;
	use Cwd 'abs_path';
	my $savedDir = abs_path (getcwd ());

	chdir ("../../IntermediateFiles/Platform_Linux/Debug/");
	if ($useBld eq "rebuild") {
		print ("Samples_SampleService; clobber...\n");
		system ("cd Samples_SampleService; make -s clobber");
		$useBld = "all";
	}
	
	print ("Samples_SampleService; $useBld...\n");
	system ("cd Samples_SampleService; make -s $useBld");
	chdir ($savedDir);
}
if ("$^O" eq "cygwin") {
	foreach (@kConfigurations) {
		my $curConfig	=	$_;
		my $extraArgs = GetMSBuildArgs();
		RunAndPrint ("cd $useProjectDir; msbuild.exe $extraArgs SampleService.sln /p:$curConfig /target:$useBld");
	}
}

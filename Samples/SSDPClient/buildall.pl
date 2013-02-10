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

print("Building Samples/SSDPClient...\n");
if ("$^O" eq "linux") {
	use Cwd;
	use Cwd 'abs_path';
	my $savedDir = abs_path (getcwd ());

	chdir ("../IntermediateFiles/Platform_Linux/Debug/");
	if ($useBld eq "rebuild") {
		print ("Samples_SSDPClient; clobber...\n");
		system ("cd Samples_SSDPClient; make -s clobber");
		$useBld = "all";
	}
	
	print ("Samples_SSDPClient; $useBld...\n");
	system ("cd Samples_SSDPClient; make -s $useBld");
	chdir ($savedDir);
}
else if ("$^O" eq "cygwin") {
	foreach (@kConfigurations) {
		my $curConfig	=	$_;
		my $extraArgs = GetMSBuildArgs();
		RunAndPrint ("cd $useProjectDir; msbuild.exe $extraArgs SSDPClient.sln /p:$curConfig /target:$useBld");
	}
}

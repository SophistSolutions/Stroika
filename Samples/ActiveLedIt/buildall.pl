#!/usr/bin/perl 

require "../Scripts/buildallHelpers.pl";

my $useBld = NormalizeBuildArg ($ARGV[0]);

my @kConfigurations = (	
					"Configuration=Debug-U-32,Platform=Win32",
					"Configuration=Release-U-32,Platform=Win32",
					);


print("Building Samples/ActiveLedIt...\n");
if ("^0" eq "cygwin") {
	foreach (@kConfigurations) {
		my $curConfig	=	$_;
		my $extraArgs = GetMSBuildArgs();
		RunAndPrint ("cd Projects; msbuild.exe $extraArgs ActiveLedIt.sln /p:$curConfig /target:$useBld");
	}
}

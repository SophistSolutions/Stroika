#!/usr/bin/perl 

require "../Scripts/buildallHelpers.pl";

my $useBld = NormalizeBuildArg ($ARGV[0]);

my @kConfigurations = (	
					"Configuration=Debug-U-32,Platform=Win32",
					"Configuration=Release-U-32,Platform=Win32",
					);


print("Building Samples/LedLineIt...\n");
if ("$^O" eq "cygwin") {
	foreach (@kConfigurations) {
		my $curConfig	=	$_;
		my $extraArgs = GetMSBuildArgs();
		RunAndPrint ("cd Projects/VisualStudio.Net-2010; msbuild.exe $extraArgs LedLineIt.sln /p:$curConfig /target:$useBld");
	}
}

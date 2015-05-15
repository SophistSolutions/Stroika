#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

require "SetupBuildCommonVars.pl";

my $EXTRA_MSBUILD_ARGS = "/nologo /v:quiet /clp:Summary";


my $useBld =	$BLD_TRG;
if (lc ($useBld) eq "clobber") {
	$useBld = "Clean";
}


use IPC::Open3;
use Symbol qw(gensym);
use IO::File;
local *CATCHERR = IO::File->new_tmpfile;


sub RunAndPrint
{
	my $cmd2Run = $_[0];
	print ("      $cmd2Run...\n");
	my $result = system ($cmd2Run);
	if ($result != 0) {
		die ("Run result = $result\r\n");
	}
}


my @kConfigurations = (
					"Configuration=Debug-U-32,Platform=Win32",
					"Configuration=Debug-U-64,Platform=x64",
					"Configuration=Release-U-32,Platform=Win32",
					"Configuration=Release-U-64,Platform=x64",
					"Configuration=Release-Logging-U-32,Platform=Win32",
					"Configuration=Release-Logging-U-64,Platform=x64",
					"Configuration=Release-DbgMemLeaks-U-32,Platform=Win32"
					);


print("   Building Stroika-Foundation...\n");
foreach (@kConfigurations) {
	my $curConfig	=	$_;
	RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Foundation.vcxproj /p:$curConfig /target:$useBld");
}


print("   Building Stroika-Frameworks...\n");
foreach (@kConfigurations) {
	my $curConfig	=	$_;
	RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Frameworks-Led.vcxproj /p:$curConfig /target:$useBld");
}
foreach (@kConfigurations) {
	my $curConfig	=	$_;
	RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Frameworks-Service.vcxproj /p:$curConfig /target:$useBld");
}
foreach (@kConfigurations) {
	my $curConfig	=	$_;
	RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Frameworks-WebServer.vcxproj /p:$curConfig /target:$useBld");
}
foreach (@kConfigurations) {
	my $curConfig	=	$_;
	RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Frameworks-WebService.vcxproj /p:$curConfig /target:$useBld");
}
foreach (@kConfigurations) {
	my $curConfig	=	$_;
	RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Frameworks.vcxproj /p:$curConfig /target:$useBld");
}

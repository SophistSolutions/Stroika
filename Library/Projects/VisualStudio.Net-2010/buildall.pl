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
	print ("$cmd2Run...\n");
	my $result = system ($cmd2Run);
	if ($result != 0) {
		print "Run result = $result\r\n";
	}
}


my @kConfigurations = (	
					#ANSI
					"Configuration=Debug-A-32,Platform=Win32",
					"Configuration=Release-A-32,Platform=Win32",
					
					#UNICODE
					"Configuration=Debug-U-32,Platform=Win32",
					"Configuration=Debug-U-64,Platform=x64",
					"Configuration=Release-U-32,Platform=Win32",
					"Configuration=Release-U-64,Platform=x64",
					"Configuration=Release-Logging-U-32,Platform=Win32",
					"Configuration=Release-Logging-U-64,Platform=x64",
					"Configuration=Release-DbgMemLeaks-U-32,Platform=Win32"
					);


print("Building Stroika-Foundation...\n");
foreach (@kConfigurations) {
	my $curConfig	=	$_;
	RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Foundation.vcxproj /p:$curConfig /target:$useBld");
}


print("Building Stroika-Frameworks...\n");

foreach (@kConfigurations) {
	my $curConfig	=	$_;
	RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Framework-Led.vcxproj /p:$curConfig /target:$useBld");
}
foreach (@kConfigurations) {
	my $curConfig	=	$_;
	RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Framework-Service.vcxproj /p:$curConfig /target:$useBld");
}
foreach (@kConfigurations) {
	my $curConfig	=	$_;
	RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Framework-WebServer.vcxproj /p:$curConfig /target:$useBld");
}
foreach (@kConfigurations) {
	my $curConfig	=	$_;
	RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Framework.vcxproj /p:$curConfig /target:$useBld");
}

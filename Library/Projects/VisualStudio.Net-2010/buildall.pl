#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

require "SetupBuildCommonVars.pl";

my $EXTRA_MSBUILD_ARGS = "/nologo /v:quiet /clp:Summary";


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

# ANSI
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Foundation.sln /p:Configuration=Debug-A-32,Platform=Win32 /target:$BLD_TRG");
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Foundation.sln /p:Configuration=Release-A-32,Platform=Win32 /target:$BLD_TRG");

#UNICODE
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Foundation.sln /p:Configuration=Debug-U-32,Platform=Win32 /target:$BLD_TRG");
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Foundation.sln /p:Configuration=Debug-U-64,Platform=x64 /target:$BLD_TRG");
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Foundation.sln /p:Configuration=Release-U-32,Platform=Win32 /target:$BLD_TRG");
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Foundation.sln /p:Configuration=Release-U-64,Platform=x64 /target:$BLD_TRG");

RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Foundation.sln /p:Configuration=Release-Logging-U-32,Platform=Win32 /target:$BLD_TRG");
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Foundation.sln /p:Configuration=Release-Logging-U-64,Platform=x64 /target:$BLD_TRG");

RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Foundation.sln /p:Configuration=Release-DbgMemLeaks-U-32,Platform=Win32 /target:$BLD_TRG");

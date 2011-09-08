#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

require "../../../SetupBuildCommonVars.pl";

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

#(other targets NYI - just 2 for now -- LGP 2011-06-`8)
my $solutionName	=	"Test5.sln";


# ANSI
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS $solutionName /p:Configuration=Debug-A-32,Platform=Win32 /target:$useBld");
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS $solutionName /p:Configuration=Release-A-32,Platform=Win32 /target:$useBld");

#UNICODE
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS $solutionName /p:Configuration=Debug-U-32,Platform=Win32 /target:$useBld");
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS $solutionName /p:Configuration=Debug-U-64,Platform=x64 /target:$useBld");
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS $solutionName /p:Configuration=Release-U-32,Platform=Win32 /target:$useBld");
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS $solutionName /p:Configuration=Release-U-64,Platform=x64 /target:$useBld");

#RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS $solutionName /p:Configuration=Release-Logging-U-32,Platform=Win32 /target:$useBld");
#RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS $solutionName /p:Configuration=Release-Logging-U-64,Platform=x64 /target:$useBld");

#RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS $solutionName /p:Configuration=Release-DbgMemLeaks-U-32,Platform=Win32 /target:$useBld");
